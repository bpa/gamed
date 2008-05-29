#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <gamed/game.h>
#include "server.h"

char rand_state[8];
char buff[1024];

long get_random(long max);
void game_over(GameInstance *g);
void tellf_player(Player *p, const char *fmt, ...);
void tell_player(Player *p, const char *fmt, size_t);
void tellf_all(GameInstance *g, const char *fmt, ...);
void tell_all(GameInstance *g, const char *fmt, size_t);
void add_timer(GameInstance *g, struct timeval *period, bool persistent);
extern void handle_timer(int sock, short event, void *args);

int write_buff(const char *fmt, va_list ap);

Server server_funcs = {
	&get_random,
	&game_over,
	&tellf_player,
	&tell_player,
	&tellf_all,
	&tell_all,
	&add_timer
	};

/******************************************************************************/

long get_random(long max) {
    return random() % max;
}
/******************************************************************************/

void game_over(GameInstance *g) {
	Player *first, *next;
	first = LIST_FIRST(&g->players);
	while (first != NULL) {
		next = LIST_NEXT(first, player);
		LIST_REMOVE(first, player);
		((Client *)first)->game = NULL;
		first = next;
	}
	g->playing = 0;
	if (g->game->destroy != NULL) {
		g->game->destroy(g, &server_funcs);
	}
	else {
		if (g->data != NULL) free(g->data);
	}
    LIST_REMOVE(g, game_instance);
	free(g);
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

void tellf_all(GameInstance *g, const char *fmt, ...) {
    va_list ap;
    int len;

    va_start(ap, fmt);
    len = write_buff(fmt, ap);
    va_end(ap);

    tell_all(g, &buff[0], len);
}
/******************************************************************************/

void tell_all (GameInstance *g, const char *msg, size_t len) {
    Player *p, *tmp;
    if (len == 0) {
        len = strlen(msg);
    }
    LIST_FOREACH_SAFE(p, &g->players, player, tmp) {
        if (send(p->sock, msg, len, MSG_NOSIGNAL) == -1) {
            g->game->player_quit(g, &server_funcs, p);
        }
    }
}
/******************************************************************************/

void add_timer (GameInstance *g, struct timeval *period, bool persistent) {
    event_set(&g->timer, 0, EV_TIMEOUT | EV_PERSIST, &handle_timer, g);
    event_add(&g->timer, period);
}
/******************************************************************************/

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
/******************************************************************************/
