#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <sys/queue.h>
#include <gamed/game.h>
#include <SpeedRisk/protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

LIST_HEAD(theme_list, theme);

typedef struct theme {
	int claimed;
	char *name;
	LIST_ENTRY(theme) themes;
} Theme;

typedef struct {
    Player *player;
    unsigned int countries_held;
    unsigned int armies;
	Theme *theme;
    bool ready;
} SR_Player;

typedef struct {
	int bonus;
	int territories;
	int *required;
} Bonus;

typedef struct {
	int max_players;
	int territories;
	char *borders;
	int *starting_armies;
	int regions;
	Bonus *bonuses;
	int army_generation_period;
} Board;

typedef struct {
	Board             *board;
	SR_Player         *players;
	struct theme_list themes;
	int               army_generation_period;
	SR_Game_Status    status;
} SpeedRiskData;

void game_init     (GameInstance *g, const Server *s, Board *board);

#ifdef __cplusplus
}
#endif

#endif
