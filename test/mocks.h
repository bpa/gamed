#ifndef GAMED_MOCKS
#define GAMED_MOCKS

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <gamed/game.h>
#include <gamed/cards.h>

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

void tell_all (Game *g, const char *msg, size_t len) {
    if (len == 0) {
        len = strlen(msg);
    }
    mock_all_len[mock_all_pos] = len;
    memcpy(mock_all_buff[mock_all_pos], msg, len);
    mock_all_pos++;
    if (mock_all_pos==5) mock_all_pos = 0;
}

void shuffle   (Deck *) {}
void add_timer (Game *g, struct timeval *tv, bool persistent) { }

#endif
