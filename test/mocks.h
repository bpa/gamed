#ifndef GAMED_MOCKS
#define GAMED_MOCKS

#include <string.h>
#include <gamed/game.h>

char mock_plr_buff[5][1024];
char mock_all_buff[5][1024];
int  mock_plr_pos;
int  mock_all_pos;

void reset_mocks() {
    mock_plr_pos = 0;
    mock_all_pos = 0;
}

void tell_player (Player *p, const char *msg) {
    strncpy(mock_plr_buff[mock_plr_pos], msg, 1024);
    mock_plr_pos++;
    if (mock_plr_pos==5) mock_plr_pos = 0;
}

//void tellf_player(Player *p, const char *, ...);

void tell_all (Game *g, const char *msg) {
    strncpy(mock_all_buff[mock_all_pos], msg, 1024);
    if (mock_all_pos==5) mock_all_pos = 0;
}

//void tellf_all(Game *g, const char *, ...);

#endif
