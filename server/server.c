#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include "server.h"

static void listen_on_port(int port);
static void handle_sig_hup(int sock, short event, void *args);
static void handle_connect(int sock, short event, void *args);
static void handle_network_event(int sock, short event, void *args);
static struct event *ev_connection;
static struct event *ev_sig_hup;
static GamedCommand invalid;
extern Server server_funcs;
extern command_func player_commands[];
extern command_func game_commands[];
struct event_base *ev_base;

/******************************************************************************/

GameModuleList game_module_list;

/******************************************************************************/

void init_server(int port, const char *config_file) {
  openlog("gamed", LOG_CONS, LOG_USER);
  LIST_INIT(&game_module_list);
  ev_base = event_base_new();
  initstate(time(NULL), &rand_state[0],8);
  listen_on_port(port);
  signal(SIGPIPE,SIG_IGN);
  ev_sig_hup = event_new(ev_base, SIGHUP, EV_SIGNAL | EV_PERSIST, handle_sig_hup, NULL);
  event_add(ev_sig_hup, NULL);
  bzero(&invalid, sizeof(GamedCommand));
  invalid.command = CMD_INVALID;
  read_config(config_file);
}

/******************************************************************************/

void run_server() {
  event_base_dispatch(ev_base);
}

/******************************************************************************/

void run_as_daemon() {
  if (daemon(1, 0) == -1) {
    perror("daemonize");
    exit(0);
    /* return EX_OSERR; */
  }
  run_server();
}

/******************************************************************************/

void listen_on_port(int port) {
    int listener;
    struct sockaddr_in sa;
    socklen_t sa_len;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }
    sa_len = sizeof(sa);
    bzero(&sa, sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stderr, "Binding to port %i\n", port);
    if (bind(listener, (struct sockaddr*)&sa, sa_len) < 0) {
        perror("Unable to bind");
        exit(1);
    }
    if (listen(listener,5) < 0) {
        perror("Listen failed: ");
        exit(1);
    }
    ev_connection = event_new(ev_base, listener, EV_READ | EV_PERSIST,
         handle_connect, NULL);
    event_add(ev_connection, NULL);
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
    client = (Client*)malloc(sizeof(Client));
    bzero(client,sizeof(Client));
    client->sock = sock;
	client->ev = event_new(ev_base, sock, EV_READ | EV_PERSIST,
        &handle_network_event, client);
    event_add(client->ev, NULL);
    /* TODO add to authenticating list */
    /* printf("Received connection\n"); */
}

/******************************************************************************/

#define FUNC_RESPONSE(sentinel, funcs) \
    if (cmd->subcmd < sentinel) { \
        funcs[cmd->subcmd](client, r); \
    } \
    else { \
        if (send(sock, &invalid, 4, MSG_DONTWAIT) == -1) { \
            drop_client(client, -1); \
        } \
    }

void handle_network_event(int sock, short event, void *args) {
    int r;
    GamedCommand *cmd;
    Client *client = (Client*)args;

    /* TODO
     * This really needs to handle evil messages (the client lying about length)
     * The problems that needs to be solved properly are:
     *   1. Messages stacking up due to a busy server
     *   2. Messages broken up into pieces (reading a large block won't get the full message)
     *   3. Messages with invalid length (length > 0 but no following content)
     */
    r = recv(sock, buff, 4, 0);
    if (r > 0) {
        cmd = (GamedCommand *)&buff[0];
        if (cmd->length) {
            r = recv(sock, &buff[4], ntohs(cmd->length), 0);
            if (r > 0) {
            /* XXX this could actually be an invalid assumption */
                r = r + 4;
            }
            else {
                drop_client(client, r);
            }
        }
        buff[r] = '\0';
		/*
        fprintf(stderr,"%i %i %i:", buff[0], buff[1], (unsigned short)buff[2]);
        for(i=4;i<r+4;i++) {
            fprintf(stderr," %u", buff[i]);
        }
        fprintf(stderr,"\n");
		*/
        switch (cmd->command) {
            case CMD_NOP:
                if (send(sock, 0x0000, 4, MSG_DONTWAIT) == -1) {
                    drop_client(client, -1);
                }
                break;
            case CMD_CHAT:
                break;
            case CMD_PLAYER:
                FUNC_RESPONSE(CMD_PLAYER_SENTINEL, player_commands);
                break;
            case CMD_GAME:
                FUNC_RESPONSE(CMD_GAME_SENTINEL, game_commands);
                break;
            case CMD_ADMIN:
                break;
        }
    }
    else {
        drop_client(client, r);
    }
}

/******************************************************************************/
void drop_client(Client *client, int r) {
    int sock = client->sock;
    shutdown(sock, SHUT_RDWR);
    if (event_initialized(client->ev)) {
        event_del(client->ev);
        event_free(client->ev);
    }
    if (client->game != NULL) {
        client->game->instance.playing--;
        LIST_REMOVE(client, player_entry);
        client->game->module->game.player_quit(
            (GameInstance*)client->game, &server_funcs, (Player*)client);
    }
    /*****************************
     * Add this when chat is ready
    LIST_REMOVE(client, chat_player);
     *****************************/
    free(client);
    /*
    if (r == 0) {
        fprintf(stderr,"Socket closed\n");
    }
    else { */ /* r == -1 */ /*
        perror("Closed socket - recv");
    }
    */
}

/******************************************************************************/

void handle_timer (int sock, short event, void *args) {
    GameModuleInstance *g = (GameModuleInstance *)args;
    if (g->instance.state->timer_event != NULL) {
        g->instance.state->timer_event((GameInstance*)g, &server_funcs);
        if (g->timer_is_persistent) {
            event_add(g->timer, &g->period);
        }
    }
    else {
        if (event_initialized(g->timer)) {
            event_del(g->timer);
        }
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
