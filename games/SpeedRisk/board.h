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
    SR_Game_Status status;
    SR_Player      players[SR_MAX_PLAYERS];
    int            army_generation_time;
} SpeedRiskData;

#ifdef __cplusplus
}
#endif

#endif
