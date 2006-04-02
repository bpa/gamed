#ifndef SPEED_RISK_BOARD
#define SPEED_RISK_BOARD

#include <gamed/game.h>
#include <gamed/player.h>
#include <SpeedRisk/protocol.h>

#define SR_GENERATION_PERIOD 30
#define SR_STARTING_ARMIES   20
#define SR_MAX_PLAYERS        6

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
    SR_Player      players[SR_MAX_PLAYERS];
    int            state;
} SpeedRiskData;

bool borders(int, int);
void build_border_table();
void produce_armies(Game *game);

#endif
