#ifndef SPEED_RISK_PROTOCOL
#define SPEED_RISK_PROTOCOL

#include "stdint.h"

#define SR_NUM_COUNRIES 42
typedef enum SR_COUNTRY {
    SR_NONE=0,

// North America
    SR_EASTERN_US, SR_NORTHWEST_TERRITORY, SR_WESTERN_US, SR_ONTARIO,
    SR_CENTRAL_AMERICA, SR_ALBERTA, SR_GREENLAND, SR_ALASKA, SR_QUEBEC,
// South America
    SR_BRAZIL, SR_VENEZUELA, SR_ARGENTINA, SR_PERU,
// Europe
    SR_ICELAND, SR_SOUTHERN_EUROPE, SR_UKRAINE, SR_SCANDINAVIA,
    SR_GREAT_BRITAIN, SR_WESTERN_EUROPE,
// Africa
    SR_EGYPT, SR_CONGO, SR_MADAGASCAR,
    SR_SOUTH_AFRICA, SR_EAST_AFRICA, SR_NORTH_AFRICA,
// Asia
    SR_AFGHANISTAN, SR_MONGOLIA, SR_URAL, SR_JAPAN, SR_IRKUTSK, SR_INDIA,
    SR_SIAM, SR_YAKUTSK, SR_SIBERIA, SR_CHINA, SR_KAMCHATKA, SR_MIDDLE_EAST,
// Australia
    SR_NEW_GUINEA, SR_INDONESIA, SR_WESTERN_AUSTRALIA, SR_EASTERN_AUSTRALIA
} SR_COUNTRY;

typedef enum SR_COMMAND {
    SR_CMD_NOP = 0,
    SR_CMD_MESSAGE,
    SR_CMD_ERROR,
    SR_CMD_READY,
    SR_CMD_NOTREADY, //A.K.A. Pause
    SR_CMD_BEGIN,
    SR_CMD_MOVE,
    SR_CMD_ATTACK,
    SR_CMD_PLACE,
    SR_CMD_ATTACK_RESULT,
    SR_CMD_GAME_STATUS,
    SR_CMD_PLAYER_STATUS,
    SR_CMD_COUNTRY_STATUS,
    SR_CMD_DEFEAT,
    SR_CMD_VICTORY,
} SR_COMMAND;

typedef enum SR_ERROR {
    SR_ERR_INVALID_CMD=0,
    SR_ERR_NOT_ENOUGH_PLAYERS,
} SR_ERROR;

typedef struct {
    int8_t command;
    int8_t error;
    int16_t unused;
} SR_Error;

typedef struct {
    int8_t command;
    int8_t player;
    int8_t country;
    int8_t country2;
} SR_Status;

typedef struct {
    int8_t command;
    int8_t from;
    int8_t to;
    int8_t armies;
} SR_Command;

typedef struct {
    int8_t  command;
    int8_t  owner;
    int16_t armies;
} SR_Country;

#endif
