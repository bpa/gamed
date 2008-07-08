#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <string.h>
#include <stddef.h>
#include <gamed/queue.h>
#include <gamed/event.h>
#include <gamed/player.h>

__BEGIN_DECLS

typedef struct st_game_instance GameInstance;

typedef struct st_server {
    long (*random)(long max);
    void (*game_over)(GameInstance *g);
    void (*tellf_player)(Player *p, const char *, ...);
    void (*tell_player) (Player *p, const char *, size_t len);
    void (*tellf_all)(GameInstance *g, const char *, ...);
    void (*tell_all) (GameInstance *g, const char *, size_t len);
    void (*add_timer)(GameInstance *g, struct timeval *period, bool persistent);
} Server;

#define CHANGE_STATE(game, new_state) \
    if (game->state != NULL && game->state->leave_state != NULL) game->state->leave_state(game); \
	if (event_pending(game->timer)) game->state->timer_event(game); \
	event_del(game->timer); \
    game->state = new_state; \
    if (game->state->enter_state != NULL) game->state->enter_state(game);
    
typedef struct st_state {
    void (*enter_state) (GameInstance *g, Server *s);
    void (*player_event)(GameInstance *g, Server *s, Player *p, const char *, int len);
    void (*timer_event) (GameInstance *g, Server *s);
    void (*leave_state) (GameInstance *g, Server *s);
} State;

#define STANDARD_GAMED_GAME 1

typedef struct st_game {
	int  gamed_abi_version;
	char name[32];
	char version[32];
	void (*create)     (GameInstance *g, Server *s);
	void (*destroy)    (GameInstance *g, Server *s);
	bool (*player_join)(GameInstance *g, Server *s, Player *p);
	void (*player_quit)(GameInstance *g, Server *s, Player *p);
    LIST_ENTRY(st_game) game;
	/* TODO: Add config member */
} Game;

struct st_game_instance {
    char name[32];
	Game *game;
    State *state;
    LIST_ENTRY(st_game_instance) game_instance;
    PlayerList players;
    int playing;
    struct event timer;
	struct timeval period;
	int timer_is_persistent;
    char *data;
};

__END_DECLS

#endif
