/*****************************************************************************
 * #defines and anything helpful for a game
 *****************************************************************************/
#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <string.h>
#include <stddef.h>
#include <gamed/queue.h>
#include <gamed/event.h>

extern char game_tell_buff[1024];
extern int  game_tell_len;

LIST_HEAD(st_player_list_head, st_player);
typedef struct st_player_list_head PlayerList;

typedef struct st_player {
    int sock;
    char name[16];
    int in_game_id;
    LIST_ENTRY(st_player) player;
} Player;

typedef struct st_game_instance GameInstance;
typedef struct st_server {
    long (*random)(long max);
    void (*tellf_player)(Player *p, const char *, ...);
    void (*tell_player) (Player *p, const char *, size_t len);
    void (*tellf_all)(GameInstance *g, const char *, ...);
    void (*tell_all) (GameInstance *g, const char *, size_t len);
    void (*add_timer)(GameInstance *g, struct timeval *period, bool persistent);
} Server;

typedef struct st_game {
    void (*game_init)     (GameInstance *g);
    bool (*player_join)   (GameInstance *g, Player *p);
    void (*player_quit)   (GameInstance *g, Player *p);
    void (*handle_request)(GameInstance *g, Player *p, char *, int len);
    void (*handle_timer)  (int sock, short event, void *args);
    /*****************************************************
     * I may decide to use these in the future
    void (*player_kicked) (Game *g, Player *p, char *why);
    void (*player_dropped)(Game *g, Player *p);
     *****************************************************/
} Game;

struct st_game_instance {
    char name[32];
    Game *game;
    Server *functions;
    LIST_ENTRY(st_game_instance) game_instance;
    PlayerList players;
    int playing;
    struct event timer;
    char *data;
};

#endif
