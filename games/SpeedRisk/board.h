#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <gamed/player.h>
#include <SpeedRisk/protocol.h>

enum SR_GAME_STATE {
    SR_WAITING_FOR_PLAYERS=0,
    SR_PLACING,
    SR_RUNNING,
    SR_DONE
};

typedef struct {
    Player *player;
    int countries_held;
    int armies;
    bool ready;
} SR_Player;

typedef struct {
    SR_Game_Status status;
    SR_Player      players[5];
    int            state;
} SpeedRiskData;

#define SR_STARTING_ARMIES 10
#define SR_MAX_BORDERING 5
//int borders;

#endif
