#ifndef GAMED_ROOK_GAME_H
#define GAMED_ROOK_GAME_H

#include <stdlib.h>
#include <gamed/card.h>
#include <gamed/game.h>
#include "deck.h"

__BEGIN_DECLS

extern State WAITING;
extern State PICKING;
extern State BIDDING;
extern State PLAYING;

#define DECKSIZE 11 * 4 + 1

#define give_error(s, p, msg) \
    msg_error.command = ROOK_CMD_ERROR; \
    msg_error.error = msg; \
    s->tell_player(p,(char*)&msg_error,4);

#define all_cmd(g, s, cmd) \
    msg_command.command = cmd; \
	s->tell_all(g, (char*)&msg_command, 4);

#define all_cmd_p(g, s, cmd, p) \
    msg_command.command = cmd; \
    msg_command.player = p; \
    s->tell_all(g,(char*)&msg_command,4);

#define all_cmd_ps(g, s, cmd, p, sco) \
    msg_command.command = cmd; \
    msg_command.player = p; \
    msg_command.score = sco; \
    s->tell_all(g,(char*)&msg_command,4);

#define all_cmd_pc(g, s, cmd, p, c) \
    msg_command.command = cmd; \
    msg_command.player = p; \
    msg_command.card = c; \
    s->tell_all(g,(char*)&msg_command,4);

typedef struct st_rook_player {
    Player *player;
    bool passed;
	Hand hand;
} RookPlayer;

typedef struct st_rook_data {
    Deck deck;
    Deck discard;
    Hand nest;
    Hand pile;
    int points[2];
    int tricks[2];
    int bidder;
    int passed;
    int bid;
    int current_player;
    unsigned char trump;
    unsigned char high;
    RookPlayer players[4];
} RookData;

enum ROOK_COMMANDS {
    ROOK_CMD_ERROR = 0,
    ROOK_CMD_READY,
    ROOK_CMD_PLAYER_JOIN,
    ROOK_CMD_BIDDING,
    ROOK_CMD_BID,
    ROOK_CMD_PASS,
    ROOK_CMD_PICKING_TRUMP,
    ROOK_CMD_CHOOSE_TRUMP,
    ROOK_CMD_CHOOSE_WIDOW,
    ROOK_CMD_DEAL_HAND,
    ROOK_CMD_PLAY_CARD,
    ROOK_CMD_TAKE_TRICK,
    ROOK_CMD_ROUND_END,
    ROOK_CMD_GAME_OVER
};

enum ROOK_ERRORS {
    ROOK_ERR_INVALID_CMD = 0,
    ROOK_ERR_INVALID_BID,
    ROOK_ERR_BID_TOO_LOW,
    ROOK_ERR_BID_TOO_HIGH,
    ROOK_ERR_NOT_YOUR_TURN,
    ROOK_ERR_INVALID_TRUMP,
    ROOK_ERR_TRUMP_CHOSEN,
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
    int command :  8;
    int error   :  8;
    unsigned    : 16;
} RookError;

typedef struct {
    int score_NS : 16;
    int score_EW : 16;
} RookScore;

__END_DECLS

#endif
