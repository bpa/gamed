#ifndef GAMED_MOCKS
#define GAMED_MOCKS

#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <gamed/game.h>

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
static Server *server;

void init_utils() {
    if (!init_utils_run) {
        initstate(time(NULL), &rand_state[0], 8);
        init_utils_run = true;
    }
}

void set_random(long a, long b=-1, long c=-1, long d=-1, long e=-1) {
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

void reset_mocks() {
    int i;
    mock_plr_pos = 0;
    mock_all_pos = 0;
    for (i=0; i<5; i++) {
        memset(&mock_plr_buff[i][0], 0x00, 1024);
        memset(&mock_all_buff[i][0], 0x00, 1024);
    }
	game_quit = false;
}

void game_over (GameInstance *g) {
	Player *first, *next;
	first = LIST_FIRST(&g->players);
	while (first != NULL) {
		next = LIST_NEXT(first, player);
		LIST_REMOVE(first, player);
		first = next;
	}
	g->playing = 0;
	game_quit = true;
	if (g->game->destroy != NULL) {
		g->game->destroy(g, server);
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
    len = write_buff(fmt, ap);
    va_end(ap);

    tell_all(g, &buff[0], len);
}


void add_timer (GameInstance *g, struct timeval *tv, bool persistent) { }

void init_server(Server *s) {
	server = s;
    s->random       = &get_random;
    s->game_over    = &game_over;
    s->tellf_player = &tellf_player;
    s->tell_player  = &tell_player;
    s->tellf_all    = &tellf_all;
    s->tell_all     = &tell_all;
    s->add_timer    = &add_timer;
    init_utils();
}

#endif
