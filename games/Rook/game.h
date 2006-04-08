#ifndef GAMED_ROOK_GAME_H
#define GAMED_ROOK_GAME_H

#include <gamed/player.h>

#define DECKSIZE 14 * 4 + 1
typedef struct st_rook_player {
    Player *player;
    bool pass;
    char hand[12    + sizeof(int)];
} RookPlayer;

typedef struct st_rook_gamet {
    char deck   [DECKSIZE + sizeof(int)];
    char discard[DECKSIZE + sizeof(int)];
    char nest   [5        + sizeof(int)];
    int points  [2];
    int bidder;
    int passed;
    int state;
    int current_player;
    char trump;
    unsigned int bid;
    RookPlayer players[4];
} Rook;

enum ROOK_STATES {
    ROOK_WAITING_FOR_PLAYERS,
    ROOK_BIDDING,
    ROOK_PICKING_TRUMP,
    ROOK_PLAYING,
    ROOK_FINISHED
};

enum ROOK_COMMANDS {
    ROOK_CMD_NOP = 0,
    ROOK_CMD_ERROR = 0,
    ROOK_CMD_PLAYER_JOIN,
    ROOK_CMD_BIDDING,
    ROOK_CMD_BID,
    ROOK_CMD_PASS,
    ROOK_CMD_PICKING_TRUMP,
    ROOK_CMD_DEAL_HAND,
    ROOK_CMD_PLAY_CARD,
    ROOK_CMD_TAKE_TRICK,
    ROOK_CMD_ROUND_END,
    ROOK_CMD_GAME_OVER
};

enum ROOK_ERRORS {
    ROOK_ERR_INVALID_CMD,
    ROOK_ERR_INVALID_BID,
    ROOK_ERR_BID_TOO_LOW,
    ROOK_ERR_BID_TOO_HIGH,
    ROOK_ERR_NOT_YOUR_TURN,
    ROOK_ERR_INVALID_PLAY,
    ROOK_ERR_DONT_HOLD_CARD
};

typedef struct {
    int command    : 8;
    int player     : 8;
    unsigned card  : 8;
    unsigned score : 8;
} RookCommand;

typedef struct {
    int command    :  8;
    int error      :  8;
    unsigned none  : 16;
} RookError;

typedef struct {
    int score_NS : 16;
    int score_EW : 16;
} RookScore;

#endif
