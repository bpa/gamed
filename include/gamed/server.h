#ifndef GAMED_SERVER_H
#define GAMED_SERVER_H

#define GAMED_PORT 7483
#include <gamed/game.h>

extern char buff[1024];
struct st_chat_instance;

typedef struct st_client {
    Player player;
    GameInstance *game;
    struct st_chat_instance *chat;
    LIST_ENTRY(st_client) chat_player;
    struct event ev;
} Client;

LIST_HEAD(st_client_list_head, st_client);
typedef struct st_client_list_head ClientList;

LIST_HEAD(st_game_instance_head, st_game_instance);
typedef struct st_game_instance_head GameInstanceList;

LIST_HEAD(st_chat_instance_head, st_chat_instance);
typedef struct st_game_instance_head ChatInstanceList;

typedef struct st_chat_instance {
    char name[32];
    LIST_ENTRY(st_client) chat_instance;
    ClientList players;
} ChatInstance;

extern GameInstanceList game_list;
extern ChatInstanceList chat_list;
extern Game **available_games;

void run_server(int port);
void run_as_daemon(int port);
void handle_command(Client *, int len);

#endif
