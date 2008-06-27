#ifndef SPEED_RISK_PROTOCOL
#define SPEED_RISK_PROTOCOL

#include <stdint.h>
#include <gamed/command.h>

#define SR_NUM_COUNRIES 42
typedef enum SR_COUNTRY {
/* North America (9) */
    SR_EASTERN_US=0, SR_NORTHWEST_TERRITORY, SR_WESTERN_US, SR_ONTARIO,
    SR_CENTRAL_AMERICA, SR_ALBERTA, SR_GREENLAND, SR_ALASKA, SR_QUEBEC,
/* South America (4) */
    SR_BRAZIL, SR_VENEZUELA, SR_ARGENTINA, SR_PERU,
/* Europe (7) */
    SR_ICELAND, SR_SOUTHERN_EUROPE, SR_UKRAINE, SR_SCANDINAVIA,
    SR_GREAT_BRITAIN, SR_WESTERN_EUROPE, SR_NORTHERN_EUROPE,
/* Africa (6) */
    SR_EGYPT, SR_CONGO, SR_MADAGASCAR,
    SR_SOUTH_AFRICA, SR_EAST_AFRICA, SR_NORTH_AFRICA,
/* Asia (12) */
    SR_AFGHANISTAN, SR_MONGOLIA, SR_URAL, SR_JAPAN, SR_IRKUTSK, SR_INDIA,
    SR_SIAM, SR_YAKUTSK, SR_SIBERIA, SR_CHINA, SR_KAMCHATKA, SR_MIDDLE_EAST,
/* Australia (4) */
    SR_NEW_GUINEA, SR_INDONESIA, SR_WESTERN_AUSTRALIA, SR_EASTERN_AUSTRALIA
} SR_COUNTRY;

typedef enum SR_COMMAND {
    SR_CMD_PLAYER_JOIN = CMD_USER_GAME_START,
    SR_CMD_MESSAGE,
    SR_CMD_ERROR,
    SR_CMD_READY,
    SR_CMD_NOTREADY, /* A.K.A. Pause */
    SR_CMD_START_PLACING,
    SR_CMD_BEGIN,
    SR_CMD_MOVE,
    SR_CMD_ATTACK,
    SR_CMD_PLACE,
    SR_CMD_GET_ARMIES,
    SR_CMD_ATTACK_RESULT,
    SR_CMD_MOVE_RESULT,
    SR_CMD_GAME_STATUS,
    SR_CMD_PLAYER_STATUS,
    SR_CMD_COUNTRY_STATUS,
    SR_CMD_DEFEAT,
    SR_CMD_VICTORY,
    SR_CMD_PLAYER_QUIT
} SR_COMMAND;

typedef struct {
    int command : 8;
    unsigned from    : 8;
    unsigned to      : 8;
    unsigned armies  : 8;
} SR_Command;

typedef enum SR_ERROR {
    SR_ERR_INVALID_CMD=0,
    SR_ERR_NOT_ENOUGH_PLAYERS,
    SR_ERR_NOT_ENOUGH_ARMIES,
    SR_ERR_NOT_OWNER,
    SR_ERR_INVALID_DESTINATION
} SR_ERROR;

typedef struct {
    int command : 8;
    int error   : 8;
    int unused  : 16;
} SR_Error;

typedef struct {
    unsigned country : 8;
    unsigned owner   : 8;
    unsigned armies  : 8;
    unsigned unused  : 8;
} SR_Country;

typedef struct {
    SR_Command command;
    SR_Country countries[SR_NUM_COUNRIES];
} SR_Game_Status;

typedef struct {
    SR_Command command;
    SR_Country country;
} SR_Country_Status;

typedef struct {
    SR_Command command;
    SR_Country country1;
    SR_Country country2;
} SR_Move_Result;

#endif
