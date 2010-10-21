#ifndef SPEEDRISK_TEST_H
#define SPEEDRISK_TEST_H

#include <SpeedRisk/board.h>
#include <SpeedRisk/borders.h>
#include <SpeedRisk/protocol.h>
#include <gamed/game.h>

#ifdef __cplusplus
extern "C" {
#endif

void produce_armies (GameInstance *g, const Server *s);
extern Game SpeedRisk;
extern State SR_WAITING_FOR_PLAYERS;
extern State SR_PLACING;
extern State SR_RUNNING;
extern State SR_DONE;

#ifdef __cplusplus
}
#endif

#endif
