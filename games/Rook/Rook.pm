#include <string.h>
#include "rook.h"

static RookCommand msg_command;
static RookError   msg_error;

void game_init        (GameInstance *g, const Server *s);
void start_game_over  (GameInstance *g, const Server *s);
void player_join      (GameInstance *g, const Server *s, Player *p);
void player_quit      (GameInstance *g, const Server *s, Player *p);
void handle_waiting   (GameInstance *g, const Server *s, Player *p, const char *, int);
void handle_game_over (GameInstance *g, const Server *s, Player *p, const char *, int);

Game Rook = { GAMED_GAME, "Rook", "0.1", game_init, NULL, player_join, player_quit };

State GAME_OVER = { start_game_over, handle_game_over, NULL, NULL };

void game_init (GameInstance *g, const Server *s) {
    RookData *rook = (RookData*)malloc(sizeof(RookData));
    bzero(rook, sizeof(Rook));
    g->data = (char*)rook;
    g->state = &WAITING;
}

void start_game_over  (GameInstance *g, const Server *s) {}

void player_join (GameInstance *g, const Server *s, Player *p) {
    RookData *rook = (RookData*)g->data;
    p->in_game_id = g->playing;
    rook->players[g->playing].player = p;
    
    all_cmd_p(g, s, ROOK_CMD_PLAYER_JOIN, p->in_game_id);
    if (g->playing == 4) {
        s->change_state(g, &BIDDING);
    }
}

void player_quit (GameInstance *g, const Server *s, Player *p) {
}

void handle_game_over (GameInstance *g, const Server *s, Player *p, const char *req, int len) {}
