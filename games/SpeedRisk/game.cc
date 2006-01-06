#include <gamed/game.h>
#include <SpeedRisk/board.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

SR_Status  msg_status;
SR_Command msg_command;
SR_Country msg_country;
SR_Error   msg_err;

void give_game_status(SpeedRiskData *d, Player *p) {
    msg_country.command = SR_CMD_GAME_STATUS;
    tell_player(p, (char*)&msg_country);
}

void give_country_status(SpeedRiskData *d, Player *p, SR_Command *cmd) {
    msg_country.command = SR_CMD_COUNTRY_STATUS;
    tell_player(p, (char*)&msg_country);
}

void game_init (Game *g) {
    LIST_INIT(&(g->players));
    SpeedRiskData *data = (SpeedRiskData*)malloc(sizeof(SpeedRiskData));
    g->data = (char*)data;
    g->playing = 0;
}

bool player_join   (Game *g, Player *p);
void player_quit   (Game *g, Player *p);
void handle_request(Game *g, Player *p, char *);

bool player_join (Game *g, Player *p) {
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    if (board->state == SR_WAITING_FOR_PLAYERS) {
        if (g->playing < 5) {
            LIST_INSERT_HEAD(&g->players, p, players);
            g->playing++;
            board->players[0].player = p;
            
            tell_player(p, "Welcome to SpeedRisk\n");
            tell_all(g, " to SpeedRisk\n");
            return true;
        }
    }
    return false;
}

void player_quit (Game *g, Player *p) {
    printf("Player quit\n");
    LIST_REMOVE(p, players);
    g->playing--;
    free(g->data);
}

void handle_request (Game *game, Player *p, char *req) {
    SpeedRiskData *g = (SpeedRiskData*)game->data;
    SR_Command *cmd = (SR_Command*)req;
    switch(g->state) {
        case SR_WAITING_FOR_PLAYERS:
            switch (cmd->command) {
                case SR_CMD_READY:
                    msg_status.command = SR_CMD_READY;
                    msg_status.player = p->in_game_id;
                    tell_all(game,(char*)&msg_status);
                    break;
                case SR_CMD_NOTREADY:
                    msg_status.command = SR_CMD_NOTREADY;
                    msg_status.player = p->in_game_id;
                    tell_all(game,(char*)&msg_status);
                    break;
                default:
                    msg_err.command = SR_CMD_ERROR;
                    msg_err.error = SR_ERR_INVALID_CMD;
                    tell_player(p,(char*)&msg_err);
                    break;
            }
        case SR_RUNNING:
        case SR_PAUSED:
        case SR_DONE:
            switch (cmd->command) {
                case SR_CMD_NOP:
                    msg_command.command = SR_CMD_NOP;
                    tell_player(p,(char*)&msg_command);
                    break;
                case SR_CMD_GAME_STATUS:
                    give_game_status(g, p);
                    break;
                case SR_CMD_COUNTRY_STATUS:
                    give_country_status(g, p, cmd);
                default:
                    msg_err.command = SR_CMD_ERROR;
                    msg_err.error = SR_ERR_INVALID_CMD;
                    tell_player(p,(char*)&msg_err);
                    break;
            }
        break;
    }
}
/*
    SR_CMD_NOP = 0,
    SR_CMD_READY,
    SR_CMD_INVALID,
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
*/
