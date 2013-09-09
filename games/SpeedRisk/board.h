#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <gamed/game.h>
#include <SpeedRisk/protocol.h>

#define SR_GENERATION_PERIOD 30000 /* milliseconds */
#define SR_MAX_PLAYERS           6

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
    SR_Game_Status status;
    SR_Player      players[SR_MAX_PLAYERS];
    int            army_generation_time;
} SpeedRiskData;

void build_border_table();

#ifdef __cplusplus
}
#endif

#endif
