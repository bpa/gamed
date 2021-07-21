#ifndef SPEEDRISK_TEST_H
#define SPEEDRISK_TEST_H

#include <SpeedRisk/board.h>
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

//Start generated
#define SR_MAX_PLAYERS 6
#define SR_NUM_COUNTRIES 42
#define SR_EASTERN_US 0
#define SR_NORTHWEST_TERRITORY 1
#define SR_WESTERN_US 2
#define SR_ONTARIO 3
#define SR_CENTRAL_AMERICA 4
#define SR_ALBERTA 5
#define SR_GREENLAND 6
#define SR_ALASKA 7
#define SR_QUEBEC 8
#define SR_BRAZIL 9
#define SR_VENEZUELA 10
#define SR_ARGENTINA 11
#define SR_PERU 12
#define SR_ICELAND 13
#define SR_SOUTHERN_EUROPE 14
#define SR_UKRAINE 15
#define SR_SCANDINAVIA 16
#define SR_GREAT_BRITAIN 17
#define SR_WESTERN_EUROPE 18
#define SR_NORTHERN_EUROPE 19
#define SR_EGYPT 20
#define SR_CONGO 21
#define SR_MADAGASCAR 22
#define SR_SOUTH_AFRICA 23
#define SR_EAST_AFRICA 24
#define SR_NORTH_AFRICA 25
#define SR_AFGHANISTAN 26
#define SR_MONGOLIA 27
#define SR_URAL 28
#define SR_JAPAN 29
#define SR_IRKUTSK 30
#define SR_INDIA 31
#define SR_SIAM 32
#define SR_YAKUTSK 33
#define SR_SIBERIA 34
#define SR_CHINA 35
#define SR_KAMCHATKA 36
#define SR_MIDDLE_EAST 37
#define SR_NEW_GUINEA 38
#define SR_INDONESIA 39
#define SR_WESTERN_AUSTRALIA 40
#define SR_EASTERN_AUSTRALIA 41
//END generated
#ifdef __cplusplus
}
#endif

#endif
