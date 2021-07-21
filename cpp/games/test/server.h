#ifndef GAMED_MOCKS
#define GAMED_MOCKS

#include <gamed/game.h>

extern long next_rand[5];
extern char rand_state[8];
extern int  game_tell_len;
extern char game_tell_buff[1024];
extern char mock_plr_buff[5][1024];
extern char mock_all_buff[5][1024];
extern int  mock_plr_len[5];
extern int  mock_all_len[5];
extern int  mock_plr_pos;
extern int  mock_all_pos;
extern bool init_utils_run;
extern bool game_quit;
extern char buff[1024];

void reset_mocks();
void set_random(long a, long b=-1, long c=-1, long d=-1, long e=-1);

Server *get_server();
GameInstance *create_instance(Game *g);
void destroy_instance(GameInstance *g);
void player_join(GameInstance *g, Player *p);
void player_event(GameInstance *g, Player *p, const char *msg, int len);
void player_quit(GameInstance *g, Player *p);

#endif
