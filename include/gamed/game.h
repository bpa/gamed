/*****************************************************************************
 * #defines and anything helpful for a game
 *****************************************************************************/
#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <string.h>
#include <gamed/player.h>

extern char game_tell_buff[1024];
extern int  game_tell_len;

typedef struct {
    PlayerList players;
    int playing;
    char * data;
} Game;

void game_init     (Game *g);
bool player_join   (Game *g, Player *p);
void player_quit   (Game *g, Player *p);
void handle_request(Game *g, Player *p, char *, int len);
//void player_kicked (Game *g, Player *p, char *why);
//void player_dropped(Game *g, Player *p);

void tell_player (Player *p, const char *, int len);
void tell_all (Game *g, const char *, int len);

#define tellf_player(p, msg, ...) \
    game_tell_len = snprintf(&game_tell_buff[0], 1024, msg, ##__VA_ARGS__); \
    tell_player(p, &game_tell_buff[0], game_tell_len);

#define tellf_all(p, msg, ...) \
    game_tell_len = snprintf(&game_tell_buff[0], 1024, msg, ##__VA_ARGS__); \
    tell_all(p, &game_tell_buff[0], game_tell_len);

#endif
