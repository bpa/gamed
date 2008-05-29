#define _BSD_SOURCE
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <unistd.h>
#include "server.h"

static void listen_on_port(int port);
static void handle_sig_hup(int sock, short event, void *args);
static void handle_connect(int sock, short event, void *args);
static void handle_network_event(int sock, short event, void *args);
static struct event ev_connection;
static struct event ev_sig_hup;
extern Server server_funcs;

/******************************************************************************/

GameInstanceList game_list;
ChatInstanceList chat_list;

/******************************************************************************/

void run_server(int port) {
  openlog("gamed", LOG_CONS, LOG_USER);
  LIST_INIT(&game_list);
  LIST_INIT(&chat_list);
  event_init();
  initstate(time(NULL), &rand_state[0],8);
  listen_on_port(port);
  signal(SIGPIPE,SIG_IGN);
  event_set(&ev_sig_hup, SIGHUP, EV_SIGNAL | EV_PERSIST, handle_sig_hup, NULL);
  event_add(&ev_sig_hup,NULL);
  event_dispatch();
  /* lt_dlinit(); */
}

/******************************************************************************/

void run_as_daemon(int port) {
  if (daemon(1, 0) == -1) {
    perror("daemonize");
    exit(0);
    /* return EX_OSERR; */
  }
  run_server(port);
}

/******************************************************************************/

void listen_on_port(int port) {
    int listener;
    struct sockaddr_in sa;
    socklen_t sa_len;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        perror("socket");
        exit(0);
    }
    sa_len = sizeof(sa);
    memset(&sa,0,sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stderr, "Binding to port %i\n", port);
    if (bind(listener, (struct sockaddr*)&sa, sa_len) < 0) {
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
    char ip_str[16];
    socklen_t sa_len = sizeof(sa);
    int sock = accept(listener, (struct sockaddr*)&sa, &sa_len);
    if (sock < 0) {
        perror("Failed connection");
    }
    inet_ntop(AF_INET,&sa.sin_addr, ip_str, 15);
    fprintf(stderr, "Recieved connection from %s\n", ip_str);
    client = (Client*)malloc(sizeof(Client));
    bzero(client,sizeof(Client));
    client->player.sock = sock;
    event_set((struct event*)&client->ev, sock, EV_READ | EV_PERSIST,
        &handle_network_event, client);
    event_add((struct event*)&client->ev,NULL);
	/* TODO add to authenticating list */
}

/******************************************************************************/

void handle_network_event(int sock, short event, void *args) {
    int r;
    Client *client = (Client*)args;
    r = recv(sock, buff, 1023,0);
    buff[r] = '\0';
    if (r > 0) {
        switch (buff[0]) {
            case '/':
                handle_command(client, r);
                break;
            case ':':
                /* client->chat->(client, r); */
                break;
            default:
                if (client->game != NULL) {
                    client->game->state->player_event(client->game, &server_funcs, &client->player, &buff[0], r);
                }
        }
    }
    else {
        shutdown(sock, SHUT_RDWR);
        event_del(&client->ev);
        if (client->game != NULL) {
            client->game->game->player_quit(client->game, &server_funcs, &client->player);
        }
        /*****************************
         * Add this when chat is ready
        LIST_REMOVE(client, chat_player);
         *****************************/
        free(client);
        if (r == 0) {
            fprintf(stderr,"Socket closed\n");
        }
        else { /* r == -1 */
            perror("Closed socket - recv");
        }
    }
}

/******************************************************************************/

void handle_timer (int sock, short event, void *args) {
	GameInstance *g = (GameInstance *)args;
	if (g->state->timer_event != NULL) {
		g->state->timer_event(g, &server_funcs);
	}
	else {
		event_del(&g->timer);
	}
}

/******************************************************************************/

void handle_chat (Client *c, int len) {
    printf("Chat%s\n", buff);
}

/******************************************************************************/

void handle_sig_hup(int sock, short event, void *args) {
}

/******************************************************************************/
