#include <gamed/game.h>
#include <gamed/utils.h>
#include <SpeedRisk/board.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

SR_Command msg_command;
SR_Country msg_country;
SR_Error   msg_error;

#define all_cmd_f(g, cmd, from) \
    msg_command.command = cmd; \
    tell_all(g,(char*)&msg_command,4);

#define player_cmd(p, cmd) \
    msg_command.command = cmd; \
    tell_player(p,(char*)&msg_command,4);

#define player_cmd_f(p, cmd, from) \
    msg_command.command = cmd; \
    msg_command.from = from; \
    tell_player(p,(char*)&msg_command,4);

#define player_cmd_a(p, cmd, n_armies) \
    msg_command.command = cmd; \
    msg_command.armies = n_armies; \
    tell_player(p,(char*)&msg_command,4);

#define player_error(p, msg) \
    msg_error.error = msg; \
    tell_player(p,(char*)&msg_error,4);

void init_board(Game *game) {
    int c, i, armies, player;
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    SR_Game_Status *status = &board->status;
    armies = int(SR_NUM_COUNRIES / game->playing);
    if (SR_NUM_COUNRIES % game->playing != 0) armies++;
    for (i=0; i < game->playing; i++) {
        board->players[i].ready = false;
        board->players[i].armies = armies;
        board->players[i].countries_held = 0;
    }
    for (c=0; c < SR_NUM_COUNRIES; c++) {
        status->countries[c].armies = 1;
        do {
            player = get_random(game->playing);
        }
        while (board->players[player].armies == 0);
        board->players[player].armies--;
        board->players[player].countries_held++;
        status->countries[c].owner = player;
    }
    for (i=0; i < game->playing; i++) {
        board->players[i].armies += SR_STARTING_ARMIES;
        player_cmd_a(board->players[i].player, SR_CMD_GET_ARMIES, 10);
    }
}

void give_game_status(Game *game, Player *p=NULL) {
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    if (p == NULL) {
        tell_all(game, (char*)&board->status,sizeof(SR_Game_Status));
    }
    else {
        tell_player(p, (char*)&board->status,sizeof(SR_Game_Status));
    }
}

void give_country_status(Game *game, Player *p, int country) {
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    SR_Country_Status status;
    status.command.command = SR_CMD_COUNTRY_STATUS;
    status.country.country = country;
    status.country.owner   = board->status.countries[country].owner;
    status.country.armies  = board->status.countries[country].armies;
    if (p == NULL) {
        tell_all(game, (char*)&status,sizeof(SR_Country_Status));
    }
    else {
        tell_player(p, (char*)&status,sizeof(SR_Country_Status));
    }
}

void game_init (Game *g) {
    LIST_INIT(&(g->players));
    SpeedRiskData *data = (SpeedRiskData*)malloc(sizeof(SpeedRiskData));
    g->data = (char*)data;
    data->status.command.command = SR_CMD_GAME_STATUS;
    g->playing = 0;
    msg_error.command = SR_CMD_ERROR;
    build_border_table();
}

bool player_join   (Game *g, Player *p);
void player_quit   (Game *g, Player *p);
void handle_request(Game *g, Player *p, char *, int len);

bool player_join (Game *g, Player *p) {
    Player *plr;
    LIST_FOREACH(plr, &g->players, players) {
        if (plr == p) return false;
    }
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    if (board->state == SR_WAITING_FOR_PLAYERS) {
        if (g->playing < 5) {
            LIST_INSERT_HEAD(&g->players, p, players);
            p->in_game_id = g->playing;
            board->players[g->playing].player = p;
            board->players[g->playing].ready = false;
            g->playing++;
            
            all_cmd_f(g, SR_CMD_PLAYER_JOIN, p->in_game_id);
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

void handle_request (Game *game, Player *p, char *req, int len) {
    int i;
    bool all_ready;
    SpeedRiskData *srd = (SpeedRiskData*)game->data;
    SR_Game_Status *status = &srd->status;
    SR_Command *cmd = (SR_Command*)req;
    switch(srd->state) {
        case SR_WAITING_FOR_PLAYERS:
            switch (cmd->command) {
                case SR_CMD_READY:
                    srd->players[p->in_game_id].ready = true;
                    if (game->playing > 1) {
                        all_cmd_f(game, SR_CMD_READY, p->in_game_id);
                        all_ready = true;
                        for (i=0; i< game->playing; i++) {
                            all_ready &= srd->players[i].ready;
                        }
                        if (all_ready) {
                            srd->state = SR_PLACING;
                            msg_command.command = SR_CMD_START_PLACING;
                            tell_all(game,(char*)&msg_command,4);
                            init_board(game);
                            give_game_status(game,NULL);
                        }
                    }
                    else {
                        player_error(p, SR_ERR_NOT_ENOUGH_PLAYERS);
                    }
                    break;
                case SR_CMD_NOTREADY:
                    srd->players[p->in_game_id].ready = false;
                    msg_command.command = SR_CMD_NOTREADY;
                    msg_command.from = p->in_game_id;
                    tell_all(game,(char*)&msg_command,4);
                    break;
                default:
                    player_error(p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_PLACING:
            switch (cmd->command) {
                case SR_CMD_READY:
                    srd->players[p->in_game_id].ready = true;
                    msg_command.command = SR_CMD_READY;
                    msg_command.from = p->in_game_id;
                    tell_all(game,(char*)&msg_command, 4);
                    all_ready = true;
                    for (i=0; i< game->playing; i++) {
                        all_ready &= srd->players[i].ready;
                    }
                    if (all_ready) {
                        srd->state = SR_RUNNING;
                        msg_command.command = SR_CMD_BEGIN;
                        tell_all(game,(char*)&msg_command,4);
                    }
                    break;
                case SR_CMD_NOTREADY:
                    srd->players[p->in_game_id].ready = false;
                    msg_command.command = SR_CMD_NOTREADY;
                    msg_command.from = p->in_game_id;
                    tell_all(game,(char*)&msg_command,4);
                    break;
                case SR_CMD_PLACE:
                    if (cmd->to >= SR_NUM_COUNRIES) {
                        player_error(p, SR_ERR_INVALID_CMD);
                    }
                    else if (srd->status.countries[cmd->to].owner 
                                != (unsigned int)p->in_game_id) {
                        player_error(p, SR_ERR_NOT_OWNER);
                    }
                    else if (cmd->armies > 
                        (unsigned int)srd->players[p->in_game_id].armies) {
                        player_error(p, SR_ERR_NOT_ENOUGH_ARMIES);
                    }
                    else {
                        status->countries[cmd->to].armies += cmd->armies;
                        srd->players[p->in_game_id].armies -= cmd ->armies;
                        give_country_status(game, NULL, cmd->to);
                    }
                default:
                    player_error(p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_RUNNING:
            switch (cmd->command) {
                case SR_CMD_MOVE:
                case SR_CMD_ATTACK:
                case SR_CMD_PLACE:
                case SR_CMD_GAME_STATUS:
                case SR_CMD_PLAYER_STATUS:
                case SR_CMD_COUNTRY_STATUS:
                default:
                    player_error(p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_DONE:
            switch (cmd->command) {
                case SR_CMD_NOP:
                    msg_command.command = SR_CMD_NOP;
                    tell_player(p,(char*)&msg_command,4);
                    break;
                case SR_CMD_GAME_STATUS:
                    give_game_status(game, p);
                    break;
                case SR_CMD_COUNTRY_STATUS:
                    give_country_status(game, p, cmd->from);
                default:
                    player_error(p, SR_ERR_INVALID_CMD);
                    break;
            }
        break;
    }
}
