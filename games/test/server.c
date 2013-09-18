#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <gamed/game.h>
#include <server/server.h>
#include "server.h"

long next_rand[5];
char rand_state[8];
int  game_tell_len;
char game_tell_buff[1024];
char mock_plr_buff[5][1024];
char mock_all_buff[5][1024];
int  mock_plr_len[5];
int  mock_all_len[5];
int  mock_plr_pos;
int  mock_all_pos;
bool init_utils_run = false;
bool game_quit = false;
char buff[1024];

void reset_mocks();
long get_random(long max);
void change_state(GameInstance *g, State *s);
void game_over (GameInstance *g);
void tellf_player(Player *p, const char *fmt, ...);
void tell_player (Player *p, const char *msg, size_t len);
void tellf_all(GameInstance *g, const char *fmt, ...);
void tell_all(GameInstance *g, const char *msg, size_t len);
void add_timer (GameInstance *g, int milliseconds, bool persistent) { }
void no_log    (GameInstance *g, const char *fmt, ...) { }

static Server server = {
    &get_random,
    &change_state,
    &game_over,
    &tellf_player,
    &tell_player,
    &tellf_all,
    &tell_all,
    &add_timer,
	&no_log
};

void player_join(GameInstance *g, Player *p) {
	reset_mocks();
	g->playing++;
	((GameModuleInstance*)g)->module->game.player_join(g, &server, p);
}

void player_quit(GameInstance *g, Player *p) {
	reset_mocks();
	g->playing--;
	((GameModuleInstance*)g)->module->game.player_quit(g, &server, p);
}

void set_random(long a, long b, long c, long d, long e) {
    next_rand[0] = a;
    next_rand[1] = b;
    next_rand[2] = c;
    next_rand[3] = d;
    next_rand[4] = e;
}

long get_random(long max) {
    long next, i;
    if (next_rand[0] != -1) {
        next = next_rand[0];
        for (i=0; i<4; i++) {
            next_rand[i] = next_rand[i+1];
        }
        next_rand[4] = -1;
        return next % max;
    }
    return random() % max;
}

void change_state(GameInstance *g, State *s) {
    if (g->state != NULL && g->state->leave_state != NULL) {
		g->state->leave_state(g, &server);
	}
    g->state = s;
    if (s->enter_state != NULL) {
		s->enter_state(g, &server);
	}
}

void reset_mocks() {
    int i;
    mock_plr_pos = 0;
    mock_all_pos = 0;
    for (i=0; i<5; i++) {
        memset(&mock_plr_buff[i][0], 0x00, 1024);
        memset(&mock_all_buff[i][0], 0x00, 1024);
    }
	game_quit = false;
    if (!init_utils_run) {
        initstate(time(NULL), &rand_state[0], 8);
        init_utils_run = true;
    }
}

void game_over (GameInstance *g) {
	g->playing = 0;
	game_quit = true;
	if (((GameModuleInstance*)g)->module->game.destroy != NULL) {
		((GameModuleInstance*)g)->module->game.destroy(g, &server);
	}
	else {
		if (g->data != NULL) free(g->data);
	}
}

int write_buff(const char *fmt, va_list ap) {
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

void tell_player (Player *p, const char *msg, size_t len) {
    if (len == 0) {
        len = strlen(msg);
    }
    mock_plr_len[mock_plr_pos] = len;
    memcpy(mock_plr_buff[mock_plr_pos], msg, len);
    mock_plr_pos++;
    if (mock_plr_pos==5) mock_plr_pos = 0;
}

void tellf_player(Player *p, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = write_buff(fmt, ap);
    va_end(ap);

    tell_player(p, &buff[0], len);
}

void tell_all(GameInstance *g, const char *msg, size_t len) {
    if (len == 0) {
        len = strlen(msg);
    }
    mock_all_len[mock_all_pos] = len;
    memcpy(mock_all_buff[mock_all_pos], msg, len);
    mock_all_pos++;
    if (mock_all_pos==5) mock_all_pos = 0;
}

void tellf_all(GameInstance *g, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsprintf(&buff[0], fmt, ap);
    va_end(ap);

    tell_all(g, &buff[0], len);
}

GameInstance *create_instance(Game *g) {
    reset_mocks();
	GameModule *module = (GameModule *)malloc(sizeof(GameModule));
	bzero(module, sizeof(GameModule));
	memcpy(&module->game, g, sizeof(Game));
	GameModuleInstance *game = (GameModuleInstance *)malloc(sizeof(GameModuleInstance));
	bzero(game, sizeof(GameModuleInstance));
	game->module = module;
	module->game.create((GameInstance*)game, &server);
	game->instance.accepting_players = true;
	return (GameInstance*)game;
}

void destroy_instance(GameInstance *g) {
	reset_mocks();
	GameModule *module = ((GameModuleInstance *)g)->module;
	game_over(g);
	free(module);
	free(g);
}

void player_event(GameInstance *g, Player *p, const char *msg, int len) {
	reset_mocks();
	g->state->player_event(g, &server, p, msg, len);
}

Server *get_server() {
	return &server;
}

