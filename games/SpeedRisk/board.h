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
	char *required;
} Bonus;

typedef struct {
	int max_players;
	int territories;
	char *borders;
	Bonus *bonuses;
	int army_generation_period;
} Board;

typedef struct {
    SR_Game_Status status;
    SR_Player      *players;
    Board          *board;
	int            army_generation_period;
} SpeedRiskData;

void game_init     (GameInstance *g, const Server *s);

#ifdef __cplusplus
}
#endif

#endif
