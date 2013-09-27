#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <gamed/game.h>
#include <SpeedRisk/protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Player *player;
    unsigned int countries_held;
    unsigned int armies;
    bool ready;
} SR_Player;

typedef struct {
	int bonus;
	int territories;
	int *required;
} Bonus;

typedef struct {
	int max_players;
	int *starting_armies;
	int territories;
	char *borders;
	int regions;
	Bonus *bonuses;
	int army_generation_period;
} Board;

typedef struct {
    SR_Player      *players;
    Board          *board;
	int            army_generation_period;
    SR_Game_Status status;
} SpeedRiskData;

void game_init   (GameInstance *g, const Server *s, Board *board);
void player_join (GameInstance *g, const Server *s, Player *p);
void player_quit (GameInstance *g, const Server *s, Player *p);

#ifdef __cplusplus
}
#endif

#endif
