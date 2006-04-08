#include <Rook/game.h>
#include <gamed/game.h>
#include <gamed/cards.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>

static RookCommand msg_command;
static RookError   msg_error;

void init_round   (Rook *);
void do_bid       (Game *, Player *, RookCommand *);
void do_pick_trump(Game *, Player *, RookCommand *);
void do_playing   (Game *, Player *, RookCommand *);

#define give_error(p, msg) \
    msg_error.command = ROOK_CMD_ERROR; \
    msg_error.error = msg; \
    tell_player(p,(char*)&msg_error,4);

#define all_cmd(g, cmd) \
    msg_command.command = cmd; \
    tell_all(g,(char*)&msg_command,4);

#define all_cmd_p(g, cmd, p) \
    msg_command.command = cmd; \
    msg_command.player = p; \
    tell_all(g,(char*)&msg_command,4);

#define all_cmd_ps(g, cmd, p, s) \
    msg_command.command = cmd; \
    msg_command.player = p; \
    msg_command.score = s; \
    tell_all(g,(char*)&msg_command,4);

void game_init (Game *g) {
    Rook *rook = (Rook*)malloc(sizeof(Rook));
    bzero(rook, sizeof(Rook));
    g->data = (char*)rook;
    rook->bid = 0;
    rook->state = ROOK_WAITING_FOR_PLAYERS;
    rook->current_player = 0;
    Deck *deck = (Deck*)&rook->deck;
    deck->cards = DECKSIZE;
}

void init_round (Rook *rook) {
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
}

bool player_join (Game *g, Player *p) {
    Player *plr;
    LIST_FOREACH(plr, &g->players, players) {
        if (plr == p) return false;
    }
    Rook *rook = (Rook*)g->data;
    if (rook->state == ROOK_WAITING_FOR_PLAYERS) {
        if (g->playing < 4) {
            LIST_INSERT_HEAD(&g->players, p, players);
            p->in_game_id = g->playing;
            rook->players[g->playing].player = p;
            g->playing++;
            
            all_cmd_p(g, ROOK_CMD_PLAYER_JOIN, p->in_game_id);
            if (g->playing == 4) {
                all_cmd(g, ROOK_CMD_BIDDING);
                rook->state = ROOK_BIDDING;
                init_round(rook);
            }
            return true;
        }
    }
    return false;
}

void player_quit (Game *g, Player *p) {
    LIST_REMOVE(p, players);
    g->playing--;
}

void handle_request(Game *game, Player *p, char *command, int len) {
    Rook *rook = (Rook*)game->data;
    switch (rook->state) {
        case ROOK_BIDDING:
            do_bid(game, p, (RookCommand*)command);
            break;
        case ROOK_PICKING_TRUMP:
            do_pick_trump(game, p, (RookCommand*)command);
            break;
        case ROOK_PLAYING:
            do_playing(game, p, (RookCommand*)command);
            break;
        default:
            give_error(p, ROOK_ERR_INVALID_CMD);
            break;
    }
}

#define bid_next_player \
    rook->current_player++; \
    if (rook->current_player > 3) rook->current_player = 0; \
    while (rook->players[rook->current_player].pass) { \
        rook->current_player++; \
        if (rook->current_player > 3) rook->current_player = 0; \
    }

void do_bid(Game *game, Player *p, RookCommand *cmd) {
    Rook *rook = (Rook*)game->data;
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
}

void do_pick_trump(Game *game, Player *p, RookCommand *cmd) {}
void do_playing(Game *game, Player *p, RookCommand *cmd) {}
