#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <syslog.h>
#include <sysexits.h>
#include <gamed/server.h>
#include <gamed/game.h>
#include <gamed/queue.h>

static void listen_on_port(int port);
static void handle_sig_hup(int sock, short event, void *args);
static void handle_connect(int sock, short event, void *args);
static void handle_network_event(int sock, short event, void *args);
static void handle_chat (Client *c, int len);
static void tellf_player(Player *p, const char *fmt, ...);
static void tell_player(Player *p, const char *fmt, size_t);
static void tellf_all(GameInstance *g, const char *fmt, ...);
static void tell_all(GameInstance *g, const char *fmt, size_t);
static void add_timer(GameInstance *g, struct timeval *period, bool persistent);
static struct event ev_connection;
static struct event ev_sig_hup;
static char rand_state[8];

long get_random(long max);

/******************************************************************************/
GameInstanceList game_list;
ChatInstanceList chat_list;
Server server_funcs = { &get_random, &tellf_player, &tell_player,
    &tellf_all, &tell_all, &add_timer };
char buff[1024];
/******************************************************************************/

long get_random(long max) {
    return random() % max;
}

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
                handle_chat(client, r);
                break;
            default:
                if (client->game != NULL) {
                    client->game->game->handle_request(client->game, &client->player, &buff[0], r);
                }
        }
    }
    else {
        shutdown(sock, SHUT_RDWR);
        event_del(&client->ev);
        if (client->game != NULL) {
            client->game->game->player_quit(client->game, &client->player);
            if (client->game->playing == 0) {
                /*TODO check to see if timer was added before deleting */
                event_del(&client->game->timer);
                free(client->game->data);
                free(client->game);
            }
            LIST_REMOVE(client->game, game_instance);
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

void handle_chat (Client *c, int len) {
    printf("Chat%s\n", buff);
}

/******************************************************************************/

void tell_player (Player *p, const char *msg, size_t len) {
    if (len == 0) {
        len = strlen(msg);
    }
    if (send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
        /* No current recourse */
        /* TODO: add a pointer to game to player, or find
                 some other way to join player to game */
    }
}
/******************************************************************************/

void tell_all (GameInstance *g, const char *msg, size_t len) {
    Player *p, *tmp;
    if (len == 0) {
        len = strlen(msg);
    }
    LIST_FOREACH_SAFE(p, &g->players, player, tmp) {
        if (send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
            g->game->player_quit(g, p);
        }
    }
}
/******************************************************************************/

void add_timer (GameInstance *g, struct timeval *period, bool persistent) {
    printf("Adding timer\n");
    event_set(&g->timer, 0, EV_TIMEOUT | EV_PERSIST, g->game->handle_timer, g);
    event_add(&g->timer, period);
}
/******************************************************************************/

void handle_sig_hup(int sock, short event, void *args) {
}
/******************************************************************************/

static int write_buff(const char *fmt, va_list ap) {
    int wrote, intval, len = 0;
    char *p, *argptr;
    char *ptr = &buff[0];

    for (p = (char *)fmt; *p; p++) {
        if (*p != '%') {
            *ptr = *p;
            ptr++;
            len++;
        }
        else {
            switch(*++p) {
                case 'i':
                case 'd':
                    intval = va_arg(ap, int);
                    wrote = sprintf(ptr, "%i", intval);
                    ptr += wrote;
                    len += wrote;
                    break;
                case 's':
                    for (argptr = va_arg(ap, char *); *argptr; argptr++) {
                        *ptr++ = *argptr;
                        len++;
                    }
                    break;
                default:
                    *ptr++ = *p;
                    len++;
            }
        }
    }
    *ptr = '\0';
    return len;
}
/******************************************************************************/

void tellf_player(Player *p, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = write_buff(fmt, ap);
    va_end(ap);

    tell_player(p, &buff[0], len);
}
/******************************************************************************/

void tellf_all(GameInstance *g, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = write_buff(fmt, ap);
    va_end(ap);

    tell_all(g, &buff[0], len);
}

