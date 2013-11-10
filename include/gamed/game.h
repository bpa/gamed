#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <sys/types.h>
#include <sys/cdefs.h>

#ifndef __cplusplus
typedef unsigned int bool;
#define true 1
#define false 0
#endif

__BEGIN_DECLS

enum SERIALIZATION_TYPE {
	NONE, //NULL
	STRING,
	BYTE,
	ARRAY,
	HASH
};

typedef struct st_player {
	char name[32];
    int in_game_id;
	void *data;
} Player;

typedef struct st_state State;
typedef struct st_game_instance GameInstance;

typedef struct st_server {
    long (*random)(long max);
	void (*change_state)(GameInstance *g, State *s);
    void (*game_over)(GameInstance *g);
    void (*tellf_player)(Player *p, const char *, ...);
    void (*tell_player) (Player *p, const char *, size_t len);
    void (*tellf_all)(GameInstance *g, const char *, ...);
    void (*tell_all) (GameInstance *g, const char *, size_t len);
    void (*add_timer)(GameInstance *g, int milliseconds, bool persistent);
	void (*log)(GameInstance *g, const char *, ...);
} Server;

struct st_state {
    void (*enter_state) (GameInstance *g, const Server *s);
    void (*player_event)(GameInstance *g, const Server *s, Player *p, const char *, int len);
    void (*timer_event) (GameInstance *g, const Server *s);
    void (*leave_state) (GameInstance *g, const Server *s);
};

#define GAMED_GAME 2

typedef struct st_game {
	int  gamed_abi_version;
	char name[32];
	char version[32];
	void (*create)     (GameInstance *g, const Server *s);
	void (*destroy)    (GameInstance *g, const Server *s);
	void (*player_join)(GameInstance *g, const Server *s, Player *p);
	void (*player_quit)(GameInstance *g, const Server *s, Player *p);
} Game;

#define STATUS_LENGTH 64
struct st_game_instance {
    char name[32];
	char status[STATUS_LENGTH];
    State *state;
    int playing;
	int accepting_players;
    void *data;
};

__END_DECLS

#endif
