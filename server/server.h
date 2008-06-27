#ifndef SERVER_H
#define SERVER_H

#define GAMED_PORT 7483
#include <gamed/game.h>
#include <gamed/chat.h>
#include <gamed/player.h>

typedef struct st_client {
    Player player;
    GameInstance *game;
    ChatInstance *chat;
    LIST_ENTRY(st_client) client;
    struct event ev;
} Client;

LIST_HEAD(st_client_list_head, st_client);
typedef struct st_client_list_head ClientList;

LIST_HEAD(st_game_head, st_game);
typedef struct st_game_head GameList;

LIST_HEAD(st_game_instance_head, st_game_instance);
typedef struct st_game_instance_head GameInstanceList;

LIST_HEAD(st_chat_instance_head, st_chat_instance);
typedef struct st_chat_instance_head ChatInstanceList;

typedef void (*command_func)(Client *, int);

extern char rand_state[8];
extern char buff[1024];

int  write_buff(const char *fmt, va_list ap);
void init_server(int port);
void run_server();
void run_as_daemon();
void handle_command(Client *, int len);
void read_config (const char *config);

#endif
