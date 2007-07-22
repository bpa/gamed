#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <string.h>
#include <stddef.h>
#include <gamed/queue.h>
#include <gamed/event.h>

__BEGIN_DECLS

typedef struct st_player {
    int sock;
    char name[16];
    int in_game_id;
    LIST_ENTRY(st_player) player;
} Player;

LIST_HEAD(st_player_list_head, st_player);
typedef struct st_player_list_head PlayerList;
typedef struct st_game_instance GameInstance;

typedef struct st_server {
    long (*random)(long max);
    void (*tellf_player)(Player *p, const char *, ...);
    void (*tell_player) (Player *p, const char *, size_t len);
    void (*tellf_all)(GameInstance *g, const char *, ...);
    void (*tell_all) (GameInstance *g, const char *, size_t len);
    void (*add_timer)(GameInstance *g, struct timeval *period, bool persistent);
} Server;

#define CHANGE_STATE(game, new_state) \
    if (game->state != NULL && game->state->leave_state != NULL) game->state->leave_state(game); \
    game->state = new_state; \
    if (game->state->enter_state != NULL) game->state->enter_state(game);
    
typedef struct st_state {
    void (*enter_state) (GameInstance *g);
    void (*player_event)(GameInstance *g, Player *p, char *, int len);
    void (*timer_event) (GameInstance *g);
    void (*leave_state) (GameInstance *g);
} State;

struct st_game_instance {
    char name[32];
    State *state;
    Server *functions;
    LIST_ENTRY(st_game_instance) game_instance;
    PlayerList players;
    int playing;
    struct event timer;
    char *data;
};

__END_DECLS

#endif
