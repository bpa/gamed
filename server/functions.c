#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <gamed/game.h>
#include "queue.h"
#include "server.h"

char rand_state[8];
char buff[1024];

long get_random(long max);
void change_state(GameInstance *g, State *s);
void game_over(GameInstance *g);
void tellf_player(Player *p, const char *fmt, ...);
void tell_player(Player *p, const char *fmt, size_t);
void tellf_all(GameInstance *g, const char *fmt, ...);
void tell_all(GameInstance *g, const char *fmt, size_t);
void add_timer(GameInstance *g, int milliseconds, bool persistent);
void log_message(GameInstance *g, const char *fmt, ...);
extern void handle_timer(int sock, short event, void *args);

Server server_funcs = {
	&get_random,
	&change_state,
	&game_over,
	&tellf_player,
	&tell_player,
	&tellf_all,
	&tell_all,
	&add_timer,
	&log_message
	};

/******************************************************************************/

long get_random(long max) {
    return random() % max;
}
/******************************************************************************/

/* The question here is if its reasonable to call back to the module
 * The other option is to set a timer for state change
 * For now, I'll call back, but if we start seeing too much recursion,
 * it will be time to add events
 */
void change_state(GameInstance *g, State *s) {
    if (g->state != NULL && g->state->leave_state != NULL) {
		g->state->leave_state(g, &server_funcs);
	}
	if (((GameModuleInstance *)g)->timer != NULL && 
		event_pending(((GameModuleInstance *)g)->timer, EV_TIMEOUT, NULL)) {
			event_del(((GameModuleInstance *)g)->timer);
	}
    g->state = s;
    if (s->enter_state != NULL) {
		s->enter_state(g, &server_funcs);
	}
}
    
/******************************************************************************/
void game_over(GameInstance *g) {
	Client *first, *next;
	GameModuleInstance *instance = (GameModuleInstance *)g;
	first = LIST_FIRST(&instance->players);
	while (first != NULL) {
		next = LIST_NEXT(first, player_entry);
		LIST_REMOVE(first, player_entry);
		first->game = NULL;
		if (first->player.data != NULL) free(first->player.data);
		first = next;
	}
	g->playing = 0;
	if (instance->module->game.destroy != NULL) {
		instance->module->game.destroy(g, &server_funcs);
	}
	else {
		if (g->data != NULL) free(g->data);
	}
	instance->module->instances--;
	if (event_initialized(instance->timer)) {
		event_del(instance->timer);
		event_free(instance->timer);
	}
    LIST_REMOVE(instance, game_instance_entry);
	free(g);
}
/******************************************************************************/

void tellf_player(Player *p, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsnprintf(&buff[4], 1020, fmt, ap);
    va_end(ap);
    SET_CMD(CMD_GAME, CMD_MESSAGE, len, buff);
    if (send(((Client*)p)->sock, &buff[0], len+4, MSG_NOSIGNAL) == -1) {
		drop_client((Client*)p, -1);
    }
}
/******************************************************************************/

void tell_player (Player *p, const char *msg, size_t len) {
    if (p == NULL) return;
    if (len == 0) {
        len = strlen(msg);
    }
    SET_CMD(CMD_GAME, CMD_MESSAGE, len, buff);
	memcpy(&buff[4], msg, len);
    if (send(((Client*)p)->sock, &buff[0], len+4, MSG_NOSIGNAL) == -1) {
		drop_client((Client*)p, -1);
    }
}
/******************************************************************************/

void tellf_all(GameInstance *g, const char *fmt, ...) {
    Client *p, *tmp;
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = vsnprintf(&buff[4], 1020, fmt, ap);
    va_end(ap);
    SET_CMD(CMD_GAME, CMD_MESSAGE, len, buff);
    LIST_FOREACH_SAFE(p, &((GameModuleInstance*)g)->players, player_entry, tmp) {
        if (send(((Client*)p)->sock, &buff[0], len+4, MSG_NOSIGNAL) == -1) {
		    drop_client(p, -1);
        }
    }
}
/******************************************************************************/

void tell_all (GameInstance *g, const char *msg, size_t len) {
    Client *p, *tmp;
    if (len == 0) {
        len = strlen(msg);
    }
    SET_CMD(CMD_GAME, CMD_MESSAGE, len, buff);
    LIST_FOREACH_SAFE(p, &((GameModuleInstance*)g)->players, player_entry, tmp) {
        if (send(p->sock, &buff[0], 4, MSG_NOSIGNAL) == -1 ||
            send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
		    drop_client(p, -1);
        }
    }
}
/******************************************************************************/

void add_timer (GameInstance *game, int milliseconds, bool persistent) {
	GameModuleInstance *g = (GameModuleInstance *)game;
	g->period.tv_sec = milliseconds / 1000;
	g->period.tv_usec = (milliseconds % 1000) * 1000;
	g->timer_is_persistent = persistent;
    g->timer = event_new(ev_base, -1, 0, &handle_timer, g);
    event_add(g->timer, &g->period);
}
/******************************************************************************/

void log_message (GameInstance *g, const char *fmt, ...) {
    va_list ap;
	snprintf(&buff[0], 1024, "[%s] %s\n", g->name, fmt);
    va_start(ap, fmt);
    vsyslog(LOG_INFO, &buff[0], ap);
    /* vprintf(&buff[0], ap); */
    va_end(ap);
}
/******************************************************************************/
