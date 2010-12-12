use Gamed::State;

class Gamed::Game::Rook::Bidding is Gamed::State;

submethod BUILD () { 
    $.name = 'bidding';
}

method enter_state ( Gamed::Server $server, Gamed::Game $game ) {
    $game.accepting_players = False;
    $server.send( { 'state' => 'bidding' } );
}

=begin END
#include "rook.h"

static RookCommand msg_command;
static RookError   msg_error;

void start_bidding (GameInstance *g, const Server *s);
void handle_bidding (GameInstance *g, const Server *s, Player *p, const char *req, int len);

State BIDDING = { start_bidding, handle_bidding, NULL, NULL };

void start_bidding (GameInstance *g, const Server *s) {
    all_cmd(g, s, ROOK_CMD_BIDDING);
/*
    int i;
    Deck *hand;
    hand = (Deck*)rook->deck;
    shuffle(hand);
    hand = (Deck*)rook->discard;
    hand->cards = 0;
    hand = (Deck*)rook->nest;
    hand->cards = 0;
    for (i=0; i<4; i++) {
        rook->players[i].pass = false;
        hand = (Deck*)&rook->players[i].hand;
        hand->cards = 0;
    }
    rook->bid = 95;
    rook->bidder = rook->current_player;
    rook->passed = 0;
    rook->trump = '\0';
*/
}

#define bid_next_player \
    rook->current_player++; \
    if (rook->current_player > 3) rook->current_player = 0; \
    while (rook->players[rook->current_player].pass) { \
        rook->current_player++; \
        if (rook->current_player > 3) rook->current_player = 0; \
    }

void handle_bidding (GameInstance *g, const Server *s, Player *p, const char *req, int len) {
/*
    RookData *rook = (RookData*)g->data;
    if (p->in_game_id != rook->current_player) {
        give_error(p, ROOK_ERR_NOT_YOUR_TURN);
    }
    else if (cmd->command == ROOK_CMD_PASS) {
        rook->players[p->in_game_id].pass = true;
        all_cmd_p(game, ROOK_CMD_PASS, p->in_game_id);
        rook->passed++;
        bid_next_player;
        if (rook->passed == 3) {
            rook->bidder = rook->current_player;
            rook->state = ROOK_PICKING_TRUMP;
            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
        }
    }
    else if (cmd->score <= rook->bid) {
        give_error(p, ROOK_ERR_BID_TOO_LOW);
    }
    else if (cmd->score > 200) {
        give_error(p, ROOK_ERR_BID_TOO_HIGH);
    }
    else if (cmd->score % 5 != 0) {
        give_error(p, ROOK_ERR_INVALID_BID);
    }
    else {
        rook->bid = cmd->score;
        rook->bidder = p->in_game_id;
        all_cmd_ps(game, ROOK_CMD_BID, p->in_game_id, rook->bid);
        if (rook->bid == 200) {
            rook->state = ROOK_PICKING_TRUMP;
            all_cmd_ps(game, ROOK_CMD_PICKING_TRUMP, rook->bidder, rook->bid);
            return;
        }
        bid_next_player;
    }
*/
}

