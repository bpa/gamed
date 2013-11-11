#ifndef SPEED_RISK_PROTOCOL
#define SPEED_RISK_PROTOCOL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SR_COMMAND {
    SR_CMD_PLAYER_JOIN = 0,
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
    SR_CMD_PLAYER_QUIT,
    SR_CMD_NEXT_ARMY_PRODUCTION,
	SR_CMD_LIST_THEMES,
	SR_CMD_SET_THEME
} SR_COMMAND;

typedef struct {
    int command      : 8;
    unsigned from    : 8;
    unsigned to      : 8;
    unsigned armies  : 8;
} SR_Command;

typedef enum SR_ERROR {
    SR_ERR_INVALID_CMD=0,
    SR_ERR_NOT_ENOUGH_PLAYERS,
    SR_ERR_NOT_ENOUGH_ARMIES,
    SR_ERR_NOT_OWNER,
    SR_ERR_INVALID_DESTINATION,
	SR_ERR_INVALID_THEME
} SR_ERROR;

typedef struct {
    int command : 8;
    int error   : 8;
    int unused  : 16;
} SR_Error;

typedef struct {
    int command : 8;
    int player  : 8;
    int length  : 16;
} SR_Status_Command;

typedef struct {
    unsigned country : 8;
    unsigned owner   : 8;
    unsigned armies  : 8;
    unsigned unused  : 8;
} SR_Country;

typedef struct {
    SR_Status_Command command;
    SR_Country countries[1];
} SR_Game_Status;

typedef struct {
    SR_Status_Command command;
    SR_Country country;
} SR_Country_Status;

typedef struct {
    SR_Command command;
    SR_Country country1;
    SR_Country country2;
} SR_Move_Result;

#ifdef __cplusplus
}
#endif

#endif
