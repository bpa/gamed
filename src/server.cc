#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <syslog.h>
#include <sysexits.h>
#include <gamed/server.h>
#include <gamed/player.h>

using namespace Gamed;
void handle_sig_hup(int sock, short event, void *args);
void handle_connect(int sock, short event, void *args);
void handle_network_event(int sock, short event, void *args);
void handle_command(Client *, int len);
void handle_timer(int sock, short event, void *args);
struct event ev_connection;
struct event ev_sig_hup;

/******************************************************************************/
char rand_state[8];
char buff[1024];
Game game;
/******************************************************************************/

long get_random(long max) {
    return random() % max;
}
/******************************************************************************/

Gamed::Server::Server() {
  openlog("gamed", NULL, LOG_USER);
  event_init();
  initstate(time(NULL), &rand_state[0],8);
  bzero(&game, sizeof(Game));
  game_init(&game);
  signal(SIGPIPE,SIG_IGN);
  event_set(&ev_sig_hup, SIGHUP, EV_SIGNAL | EV_PERSIST, handle_sig_hup, NULL);
  event_add(&ev_sig_hup,NULL);
  //lt_dlinit();
}

/******************************************************************************/

void Gamed::Server::run_as_daemon() {
  if (daemon(1, 0) == -1) {
    perror("daemonize");
    //return EX_OSERR;
  }
  run();
}

/******************************************************************************/

void Gamed::Server::run() {
  listen_on_port(GAMED_PORT);
  event_dispatch();
}

/******************************************************************************/

void Gamed::Server::listen_on_port(int port) {
    int listener;
    struct sockaddr_in sa;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        perror("socket");
        exit(0);
    }
    socklen_t sa_len = sizeof(sa);
    memset(&sa,0,sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stderr, "Binding to port %i\n", port);
    if (bind(listener, (sockaddr*)&sa, sa_len) < 0) {
        perror("Unable to bind");
        exit(0);
    }
    if (listen(listener,5) < 0) {
        perror("Listen failed: ");
        exit(0);
    }
    event_set(&ev_connection, listener, EV_READ | EV_PERSIST,
         handle_connect, NULL);
    event_add(&ev_connection,NULL);
}

/******************************************************************************/

void handle_connect(int listener, short event, void *args) {
    Client *client;
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    int sock = accept(listener, (struct sockaddr*)&sa, &sa_len);
    if (sock < 0) {
        perror("Failed connection");
    }
    char ip_str[16];
    inet_ntop(AF_INET,&sa.sin_addr, ip_str, 15);
    fprintf(stderr, "Recieved connection from %s\n");
    client = (Client*)malloc(sizeof(Client));
    bzero(client,sizeof(Client));
    /* 
    client->game = (Game*)malloc(sizeof(Game));
    game_init(client->game);
    */
    client->game = &game;
    client->player.sock = sock;
    if (player_join(client->game, &client->player)) {
        event_set((struct event*)&client->ev, sock, EV_READ | EV_PERSIST,
            &handle_network_event, client);
        event_add((struct event*)&client->ev,NULL);
    }
    else {
        close(sock);
        free(client);
    }
}

/******************************************************************************/

void handle_network_event(int sock, short event, void *args) {
    int r;
    Client *client = (Client*)args;
    r = recv(sock, buff, 1024,0);
    if (r > 0) {
        if (buff[0] == '/') {
            handle_command(client, r);
        }
        else {
            handle_request(client->game, &client->player, &buff[0], r);
        }
    }
    else {
        shutdown(sock, SHUT_RDWR);
        event_del(&client->ev);
        player_quit(client->game, &client->player);
/*
        free(client->game);
*/
        free(client);
        if (r == 0) {
            fprintf(stderr,"Socket closed\n");
        }
        else { // r == -1
            perror("Closed socket - recv");
        }
    }
}
/******************************************************************************/

void handle_command(Client *client, int len) {
    Player *plr;
    uint16_t buff_len;
    uint16_t *ptr_len = (uint16_t*)&buff[2];
    if (strncmp("/name ", &buff[0], 6) == 0) {
        strncpy(client->player.name, &buff[6], 16);
        client->player.name[15] = '\0';
        client->player.name[len-6] = '\0';
        memset(&buff[0], 0, 4);
        buff_len = sprintf(&buff[4], "rename:%i:%s",
             client->player.in_game_id, &client->player.name[0]);
        *ptr_len = htons(buff_len);
        tell_all(client->game, &buff[0], buff_len+4);
        printf("bl: %i %s\n", buff_len, &buff[4]);
        return;
    }
    if (strncmp("/players", &buff[0], 8) == 0) {
        memset(&buff[0], 0, 4);
        buff_len = 4;
        buff_len += sprintf(&buff[4], "players");
        LIST_FOREACH(plr, &client->game->players, players) {
            buff_len += sprintf(&buff[buff_len], ":%i:%s",
                 plr->in_game_id, &plr->name[0]);
        }
        buff_len -= 4;
        *ptr_len = htons(buff_len);
        printf("bl: %i %s\n", buff_len, &buff[4]);
        tell_all(client->game, &buff[0], buff_len+4);
        return;
    }
}
/******************************************************************************/
void tell_player (Player *p, const char *msg, size_t len) {
    if (send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
        /* No current recourse */
        /* TODO: add a pointer to game to player, or find
                 some other way to join player to game */
    }
}
/******************************************************************************/

void tell_all (Game *g, const char *msg, size_t len) {
    Player *p, *tmp;
    LIST_FOREACH_SAFE(p, &g->players, players, tmp) {
        if (send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
            player_quit(g, p);
        }
    }
}
/******************************************************************************/

void add_timer (Game *g, struct timeval *period, bool persistent) {
    printf("Adding timer\n");
    event_set(&g->timer, NULL, EV_TIMEOUT | EV_PERSIST, handle_timer, g);
    event_add(&g->timer, period);
}
/******************************************************************************/

void handle_sig_hup(int sock, short event, void *args) {
}
