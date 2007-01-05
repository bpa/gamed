#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <gamed/game.h>
#include <gamed/utils.h>
#include <gamed/queue.h>
#include <SpeedRisk/board.h>
#include <SpeedRisk/SpeedRisk.h>

static SR_Command msg_command;
/* static SR_Country msg_country; */
static SR_Error   msg_error;
static SR_Move_Result msg_move;
static int STARTING_ARMIES[] = { 0, 0, 26, 21, 19, 16, 13 };

static void game_init     (GameInstance *g);
static bool player_join   (GameInstance *g, Player *p);
static void player_quit   (GameInstance *g, Player *p);
static void handle_request(GameInstance *g, Player *p, char *, int len);
static void handle_timer  (int sock, short event, void *args);

Game SpeedRisk = { &game_init, &player_join, &player_quit, &handle_request, &handle_timer };

#define all_cmd_f(g, cmd, fro) \
    msg_command.command = cmd; \
    msg_command.from = fro; \
    f->tell_all(g,(char*)&msg_command,4);

#define player_cmd(g, p, cmd) \
    msg_command.command = cmd; \
    f->tell_player(p,(char*)&msg_command,4);

#define player_cmd_f(g, p, cmd, fro) \
    msg_command.command = cmd; \
    msg_command.from = fro; \
    f->tell_player(p,(char*)&msg_command,4);

#define player_cmd_a(g, p, cmd, n_armies) \
    msg_command.command = cmd; \
    msg_command.armies = n_armies; \
    f->tell_player(p,(char*)&msg_command,4);

#define player_error(g, p, msg) \
    msg_error.error = msg; \
    f->tell_player(p,(char*)&msg_error,4);

#define holds(p, c) ((int)status->countries[c].owner == p->in_game_id)
#define not_holds(p, c) ((int)status->countries[c].owner != p->in_game_id)

#define cmp_and_swap(array, a, b, tmp) \
    if (array[a] < array[b]) { \
        tmp = array[a]; array[a] = array[b]; array[b] = tmp; \
    }

#define tell_all_mv_at_result(game, cmd, from, to) \
    memcpy(&msg_move.country1, &status->countries[from], sizeof(SR_Country)); \
    memcpy(&msg_move.country2, &status->countries[to], sizeof(SR_Country)); \
    msg_move.command.command = cmd; \
    f->tell_all(game,(char*)&msg_move,sizeof(msg_move));

#define add_armies_for_continent(start, size, value) \
    holds_continent = true; \
    for(c=start; c<start+size; c++) { \
        if ((int)board->status.countries[c].owner != i) { \
             holds_continent = false; break; } \
    } \
    if (holds_continent) { armies += value; }

void handle_timer  (int sock, short event, void *args) {
    struct timeval period;
    GameInstance *g = (GameInstance*)args;
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    if (board->state == SR_DONE) {
        event_del(&g->timer);
    }
    else {
        produce_armies(g);
        period.tv_sec = SR_GENERATION_PERIOD;
        period.tv_usec = 0;
        event_add(&g->timer, &period);
    }
}

void produce_armies(GameInstance *game) {
    bool holds_continent;
    int i, c, armies, held;
    SR_Player *p;
    Server *f = game->functions;
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    for (i=0; i<game->playing; i++) {
        p = &board->players[i];
        held = p->countries_held;
        if (held > 0) {
            if (held > 11) { armies = held / 3; }
            else           { armies = 3;        }
            add_armies_for_continent(SR_EASTERN_US,    9, 5);
            add_armies_for_continent(SR_BRAZIL,        4, 2);
            add_armies_for_continent(SR_ICELAND,       7, 5);
            add_armies_for_continent(SR_EGYPT,         6, 3);
            add_armies_for_continent(SR_AFGHANISTAN,  12, 7);
            add_armies_for_continent(SR_NEW_GUINEA,    4, 2);
            p->armies += armies;
            if (p->armies > 255) { p->armies = 255; }
            player_cmd_a(game, p->player, SR_CMD_GET_ARMIES, p->armies);
        }
    }
}

void init_board(GameInstance *game) {
    int c, i, armies, player, pass;
    Server *f = game->functions;
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    SR_Game_Status *status = &board->status;
    int playing = game->playing == 2 ? 3 : game->playing;
    armies = (SR_NUM_COUNRIES / playing);
    if (SR_NUM_COUNRIES % playing != 0) armies++;
    for (i=0; i < playing; i++) {
        board->players[i].ready = false;
        board->players[i].armies = armies;
        board->players[i].countries_held = 0;
    }
    for (c=0; c < SR_NUM_COUNRIES; c++) {
        player = c % playing;
        status->countries[c].country = c;
        status->countries[c].armies = 1;
        board->players[player].armies--;
        board->players[player].countries_held++;
        status->countries[c].owner = player;
    }
    for (pass=0; pass < 2; pass++) {
        for (c=0; c < SR_NUM_COUNRIES; c++) {
            i = get_random(SR_NUM_COUNRIES);
            player = status->countries[c].owner;
            status->countries[c].owner = status->countries[i].owner;
            status->countries[i].owner = player;
        }
    }
    for (i=0; i < game->playing; i++) {
        board->players[i].armies *= 2;
        board->players[i].armies += STARTING_ARMIES[game->playing];
        player_cmd_a(game, board->players[i].player, SR_CMD_GET_ARMIES, 
            board->players[i].armies);
    }
}

void give_game_status(GameInstance *game, Player *p) {
    Server *f = game->functions;
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    if (p == NULL) {
        f->tell_all(game, (char*)&board->status,sizeof(SR_Game_Status));
    }
    else {
        f->tell_player(p, (char*)&board->status,sizeof(SR_Game_Status));
    }
}

void give_country_status(GameInstance *game, Player *p, int country) {
    Server *f = game->functions;
    SpeedRiskData *board = (SpeedRiskData*)game->data;
    SR_Country_Status status;
    status.command.command = SR_CMD_COUNTRY_STATUS;
    status.country.country = country;
    status.country.owner   = board->status.countries[country].owner;
    status.country.armies  = board->status.countries[country].armies;
    if (p == NULL) {
        f->tell_all(game, (char*)&status,sizeof(SR_Country_Status));
    }
    else {
        f->tell_player(p, (char*)&status,sizeof(SR_Country_Status));
    }
}

void game_init (GameInstance *g) {
    SpeedRiskData *data;
    LIST_INIT(&(g->players));
    data = (SpeedRiskData*)malloc(sizeof(SpeedRiskData));
    g->data = (char*)data;
    bzero(g->data, sizeof(SpeedRiskData));
    data->status.command.command = SR_CMD_GAME_STATUS;
    g->playing = 0;
    msg_error.command = SR_CMD_ERROR;
    msg_move.command.command = SR_CMD_MOVE_RESULT;
    build_border_table();
}

bool player_join (GameInstance *game, Player *p) {
    int id;
    Player *plr;
    SR_Player *sr_player;
    SpeedRiskData *board;
    Server *f = game->functions;
fprintf(stderr, "Player joined\n");
    LIST_FOREACH(plr, &game->players, player) {
        if (plr == p) return false;
    }
    board = (SpeedRiskData*)game->data;
    if (board->state == SR_WAITING_FOR_PLAYERS) {
        if (game->playing < SR_MAX_PLAYERS) {
            LIST_INSERT_HEAD(&game->players, p, player);
            for(id=0; id<SR_MAX_PLAYERS; id++) {
                if (board->players[id].player == NULL) {
                    p->in_game_id = id;
                    break;
                }
            }
            board->players[p->in_game_id].player = p;
            board->players[p->in_game_id].ready = false;
            board->players[p->in_game_id].armies = 0;
            game->playing++;
            
            all_cmd_f(game, SR_CMD_PLAYER_JOIN, p->in_game_id);
            LIST_FOREACH(plr, &game->players, player) {
                id = plr->in_game_id;
                sr_player = &board->players[id];
                if (sr_player->ready) {
                    player_cmd_f(game, p, SR_CMD_READY, id);
                }
                else {
                    player_cmd_f(game, p, SR_CMD_NOTREADY, id);
                }
            }
            return true;
        }
    }
    return false;
}

void player_quit (GameInstance *game, Player *p) {
    Server *f = game->functions;
    SpeedRiskData *srd = (SpeedRiskData*)game->data;
    LIST_REMOVE(p, player);
    game->playing--;
    all_cmd_f(game, SR_CMD_PLAYER_QUIT, p->in_game_id);
    srd->players[p->in_game_id].player = NULL;
    if (srd->state == SR_RUNNING) {
        syslog(LOG_INFO, "%s dropped", p->name);
    }
}

void handle_request (GameInstance *game, Player *p, char *req, int len) {
    int i, defending, attacking, attack_loss, defend_loss;
    int defender;
    int attack_rolls[3];
    int defend_rolls[2];
    bool all_ready;
    struct timeval period;
    Server *f = game->functions;
    SpeedRiskData *srd;
    SR_Game_Status *status;
    SR_Command *cmd;
    srd = (SpeedRiskData*)game->data;
    status = &srd->status;
    cmd = (SR_Command*)req;
    if (    cmd->to >= SR_NUM_COUNRIES ||
          cmd->from >= SR_NUM_COUNRIES ) {
        player_error(game, p, SR_ERR_INVALID_DESTINATION);
        return;
    }
    switch(srd->state) {
        case SR_WAITING_FOR_PLAYERS:
            switch (cmd->command) {
                case SR_CMD_READY:
                    if (game->playing > 1) {
                        srd->players[p->in_game_id].ready = true;
                        all_cmd_f(game, SR_CMD_READY, p->in_game_id);
                        all_ready = true;
                        for (i=0; i< game->playing; i++) {
                            all_ready &= srd->players[i].ready;
                        }
                        if (all_ready) {
                            srd->state = SR_PLACING;
                            msg_command.command = SR_CMD_START_PLACING;
                            f->tell_all(game,(char*)&msg_command,4);
                            init_board(game);
                            give_game_status(game,NULL);
                        }
                    }
                    else {
                        player_error(game, p, SR_ERR_NOT_ENOUGH_PLAYERS);
                    }
                    break;
                case SR_CMD_NOTREADY:
                    srd->players[p->in_game_id].ready = false;
                    all_cmd_f(game, SR_CMD_NOTREADY, p->in_game_id);
                    break;
                default:
                    player_error(game, p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_PLACING:
            switch (cmd->command) {
                case SR_CMD_READY:
                    srd->players[p->in_game_id].ready = true;
                    all_cmd_f(game, SR_CMD_READY, p->in_game_id);
                    all_ready = true;
                    for (i=0; i< game->playing; i++) {
                        all_ready &= srd->players[i].ready;
                    }
                    if (all_ready) {
                        srd->state = SR_RUNNING;
                        msg_command.command = SR_CMD_BEGIN;
                        f->tell_all(game,(char*)&msg_command,4);
                        period.tv_sec = SR_GENERATION_PERIOD;
                        period.tv_usec = 0;
                        f->add_timer(game, &period, true);
                        syslog(LOG_INFO, "Starting SpeedRisk");
                    }
                    break;
                case SR_CMD_NOTREADY:
                    srd->players[p->in_game_id].ready = false;
                    all_cmd_f(game, SR_CMD_NOTREADY, p->in_game_id);
                    break;
                case SR_CMD_PLACE:
                    if (not_holds(p, cmd->to)) {
                        player_error(game, p, SR_ERR_NOT_OWNER);
                    }
                    else if (cmd->armies > 
                        (unsigned int)srd->players[p->in_game_id].armies) {
                        player_error(game, p, SR_ERR_NOT_ENOUGH_ARMIES);
                    }
                    else {
                        status->countries[cmd->to].armies += cmd->armies;
                        srd->players[p->in_game_id].armies -= cmd ->armies;
                        give_country_status(game, NULL, cmd->to);
                        player_cmd_a(game, p, SR_CMD_GET_ARMIES,
                            srd->players[p->in_game_id].armies);
                    }
                default:
                    player_error(game, p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_RUNNING:
            switch (cmd->command) {
                case SR_CMD_MOVE:
                    if (!(holds(p,cmd->from) && holds(p,cmd->to))) {
                        player_error(game, p, SR_ERR_NOT_OWNER);
                    }
                    else if (!borders(cmd->from, cmd->to)) {
                        player_error(game, p, SR_ERR_INVALID_DESTINATION);
                    }
                    else if (cmd->armies>=status->countries[cmd->from].armies){
                        player_error(game, p, SR_ERR_NOT_ENOUGH_ARMIES);
                    }
                    else {
                        status->countries[cmd->from].armies -= cmd->armies;
                        status->countries[cmd->to].armies   += cmd->armies;
                        tell_all_mv_at_result(game, SR_CMD_MOVE_RESULT,
                            cmd->from, cmd->to);
                    }
                    break;
                case SR_CMD_ATTACK:
                    if (not_holds(p, cmd->from)) {
                        player_error(game, p, SR_ERR_NOT_OWNER);
                    }
                    else if (holds(p, cmd->to)) {
                        player_error(game, p, SR_ERR_INVALID_DESTINATION);
                    }
                    else if (!borders(cmd->from, cmd->to)) {
                        player_error(game, p, SR_ERR_INVALID_DESTINATION);
                    }
                    else if (cmd->armies < 1 || cmd->armies >= 
                        status->countries[cmd->from].armies) {
                        player_error(game, p, SR_ERR_NOT_ENOUGH_ARMIES);
                    }
                    else {
                        defender = status->countries[cmd->to].owner;
                        bzero(&attack_rolls, 3*sizeof(int));
                        bzero(&defend_rolls, 2*sizeof(int));
                        attacking = cmd->armies > 3 ? 3 : cmd->armies;
                        for(i=0; i<attacking; i++) {
                            attack_rolls[i] = get_random(6) + 1;
                        }
                        defending=status->countries[cmd->to].armies>1 ? 2 : 1;
                        for(i=0; i<defending; i++) {
                            defend_rolls[i] = get_random(6) + 1;
                        }
                        cmp_and_swap(defend_rolls, 0, 1, i)
                        cmp_and_swap(attack_rolls, 0, 1, i)
                        cmp_and_swap(attack_rolls, 0, 2, i)
                        cmp_and_swap(attack_rolls, 1, 2, i)
                        if (defending > (int)cmd->armies) defending = 1;
                        attack_loss = defend_loss = 0;
                        for (i=0; i < defending; i++) {
                            if (attack_rolls[i] > defend_rolls[i]) {
                                defend_loss++;
                            } else {
                                attack_loss++;
                            }
                        }
                        status->countries[cmd->from].armies -= attack_loss;
                        status->countries[cmd->to].armies   -= defend_loss;
                        if (status->countries[cmd->to].armies == 0) {
                            attacking = cmd->armies - attack_loss;
                            status->countries[cmd->from].armies -= attacking;
                            status->countries[cmd->to].armies += attacking;
                            status->countries[cmd->to].owner = p->in_game_id;
                            srd->players[p->in_game_id].countries_held++;
                            srd->players[defender].countries_held--;
                        }
                        tell_all_mv_at_result(game, SR_CMD_ATTACK_RESULT,
                            cmd->from, cmd->to);
                        if (srd->players[defender].countries_held == 0) {
                            all_cmd_f(game, SR_CMD_DEFEAT, defender);
                            if (srd->players[defender].player != NULL) {
                                syslog(LOG_INFO, "%s defeated", srd->players[defender].player->name);
                            }
                        }
                        if (srd->players[p->in_game_id].countries_held == 42) {
                            all_cmd_f(game, SR_CMD_VICTORY, p->in_game_id);
                            srd->state = SR_DONE;
                            syslog(LOG_INFO, "%s victorious", p->name);
                            closelog();
                        }
                    }
                    break;
                case SR_CMD_PLACE:
                    if (not_holds(p, cmd->to)) {
                        player_error(game, p, SR_ERR_NOT_OWNER);
                    }
                    else if (cmd->armies > 
                        (unsigned int)srd->players[p->in_game_id].armies) {
                        player_error(game, p, SR_ERR_NOT_ENOUGH_ARMIES);
                    }
                    else {
                        status->countries[cmd->to].armies += cmd->armies;
                        srd->players[p->in_game_id].armies -= cmd->armies;
                        give_country_status(game, NULL, cmd->to);
                        player_cmd_a(game, p, SR_CMD_GET_ARMIES,
                            srd->players[p->in_game_id].armies);
                    }
                    break;
                case SR_CMD_GAME_STATUS:
                    give_game_status(game, p);
                    break;
                case SR_CMD_PLAYER_STATUS:
                    player_cmd_a(game, p, SR_CMD_PLAYER_STATUS,
                            srd->players[p->in_game_id].armies);
                    break;
                case SR_CMD_COUNTRY_STATUS:
                    give_country_status(game, p, cmd->from);
                    break;
                default:
                    player_error(game, p, SR_ERR_INVALID_CMD);
                    break;
            }
            break;
        case SR_DONE:
            switch (cmd->command) {
                case SR_CMD_NOP:
                    msg_command.command = SR_CMD_NOP;
                    f->tell_player(p,(char*)&msg_command,4);
                    break;
                case SR_CMD_GAME_STATUS:
                    give_game_status(game, p);
                    break;
                case SR_CMD_COUNTRY_STATUS:
                    give_country_status(game, p, cmd->from);
                    break;
                default:
                    player_error(game, p, SR_ERR_INVALID_CMD);
                    break;
            }
        break;
    }
}

