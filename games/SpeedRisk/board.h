#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <gamed/player.h>
#include <SpeedRisk/protocol.h>

enum SR_GAME_STATE { SR_WAITING_FOR_PLAYERS=0, SR_RUNNING, SR_PAUSED, SR_DONE };

typedef struct {
    Player *player;
    int countries_held;
    int armies;
    bool ready;
} SR_Player;

typedef struct {
    SR_Country board[SR_NUM_COUNRIES];
    SR_Player  players[5];
    int        state;
} SpeedRiskData;

#define SR_MAX_BORDERING 5
//int borders;

#endif
