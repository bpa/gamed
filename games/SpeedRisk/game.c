#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gamed/game.h>
#include "board.h"
#include "boards.h"

static SR_Command msg_command;
/* static SR_Country msg_country; */
static SR_Error   msg_error;
static SR_Move_Result msg_move;
static int STARTING_ARMIES[] = { 0, 0, 26, 21, 19, 16, 13 };

void player_join   (GameInstance *g, const Server *s, Player *p);
void player_quit   (GameInstance *g, const Server *s, Player *p);
void start_placing (GameInstance *g, const Server *s);
void start_playing (GameInstance *g, const Server *s);
void start_ending  (GameInstance *g, const Server *s);
void produce_armies(GameInstance *g, const Server *s);
void quit_game     (GameInstance *g, const Server *s);
void handle_waiting(GameInstance *g, const Server *s, Player *p, const char *, int len);
void handle_placing(GameInstance *g, const Server *s, Player *p, const char *, int len);
void handle_playing(GameInstance *g, const Server *s, Player *p, const char *, int len);
void handle_done   (GameInstance *g, const Server *s, Player *p, const char *, int len);

Game SpeedRisk = { GAMED_GAME, "SpeedRisk", "0.3", game_init_classic, NULL, player_join, player_quit };
Game UltimateRisk = { GAMED_GAME, "UltimateRisk", "0.1", game_init_ultimate, NULL, player_join, player_quit };
State SR_WAITING_FOR_PLAYERS = { NULL,          handle_waiting, NULL,            NULL };
State SR_PLACING             = { start_placing, handle_placing, NULL,            NULL };
State SR_RUNNING             = { start_playing, handle_playing, produce_armies,  NULL };
State SR_DONE                = { start_ending,  handle_done,    quit_game,       NULL };

#define all_cmd_f(g, s, cmd, fro) \
    msg_command.command = cmd; \
    msg_command.from = fro; \
    msg_command.to = 0; \
    s->tell_all(g,(char*)&msg_command,4);

#define player_cmd(s, p, cmd) \
    msg_command.command = cmd; \
    s->tell_player(p,(char*)&msg_command,4);

#define player_cmd_f(s, p, cmd, fro) \
    msg_command.command = cmd; \
    msg_command.from = fro; \
    s->tell_player(p,(char*)&msg_command,4);

#define player_cmd_a(s, p, cmd, n_armies) \
    msg_command.command = cmd; \
    msg_command.armies = n_armies; \
    s->tell_player(p,(char*)&msg_command,4);

#define player_error(s, p, msg) \
    msg_error.error = msg; \
    s->tell_player(p,(char*)&msg_error,4);

#define holds(status, p, c) (status->countries[c].owner == p->in_game_id)
#define not_holds(status, p, c) (status->countries[c].owner != p->in_game_id)

#define cmp_and_swap(array, a, b, tmp) \
    if (array[a] < array[b]) { \
        tmp = array[a]; array[a] = array[b]; array[b] = tmp; \
    }

#define tell_all_mv_at_result(g, s, cmd, from, to) \
    memcpy(&msg_move.country1, &status->countries[from], sizeof(SR_Country)); \
    memcpy(&msg_move.country2, &status->countries[to], sizeof(SR_Country)); \
    msg_move.command.command = cmd; \
    s->tell_all(g,(char*)&msg_move,sizeof(msg_move));

#define add_armies_for_continent(start, size, value) \
    holds_continent = true; \
    for(c=start; c<start+size; c++) { \
        if ((int)risk->status.countries[c].owner != i) { \
             holds_continent = false; break; } \
    } \
    if (holds_continent) { armies += value; }

void produce_armies (GameInstance *g, const Server *s) {
    bool holds_continent;
    int i, c, armies, held;
    SR_Player *p;
    SpeedRiskData *risk = (SpeedRiskData*)g->data;
    for (i=0; i<g->playing; i++) {
        p = &risk->players[i];
        held = p->countries_held;
        if (held > 0) {
            if (held > 11) { armies = held / 3; }
            else           { armies = 3;        }
            add_armies_for_continent(SR_EASTERN_US,   9, 5);
            add_armies_for_continent(SR_BRAZIL,       4, 2);
            add_armies_for_continent(SR_ICELAND,      7, 5);
            add_armies_for_continent(SR_EGYPT,        6, 3);
            add_armies_for_continent(SR_AFGHANISTAN, 12, 7);
            add_armies_for_continent(SR_NEW_GUINEA,   4, 2);
            p->armies += armies;
            if (p->armies > 255) { p->armies = 255; }
            player_cmd_a(s, p->player, SR_CMD_GET_ARMIES, p->armies);
        }
    }
	if (risk->army_generation_period > 15)
		risk->army_generation_period--;
	all_cmd_f(g, s, SR_CMD_NEXT_ARMY_PRODUCTION, risk->army_generation_period);
	s->add_timer(g, risk->army_generation_period * 1000, false);
}

void init_board(GameInstance *game, const Server *s) {
    int c, i, armies, player, pass;
    SpeedRiskData *risk = (SpeedRiskData*)game->data;
    SR_Game_Status *status = &risk->status;
    int playing = game->playing == 2 ? 3 : game->playing;
    armies = (SR_NUM_COUNRIES / playing);
    if (SR_NUM_COUNRIES % playing != 0) armies++;
    for (i=0; i < playing; i++) {
        risk->players[i].ready = false;
        risk->players[i].armies = armies;
        risk->players[i].countries_held = 0;
    }
    for (c=0; c < SR_NUM_COUNRIES; c++) {
        player = c % playing;
        status->countries[c].country = c;
        status->countries[c].armies = 1;
        risk->players[player].armies--;
        risk->players[player].countries_held++;
        status->countries[c].owner = player;
    }
    for (pass=0; pass < 2; pass++) {
        for (c=0; c < SR_NUM_COUNRIES; c++) {
            i = s->random(SR_NUM_COUNRIES);
            player = status->countries[c].owner;
            status->countries[c].owner = status->countries[i].owner;
            status->countries[i].owner = player;
        }
    }
    for (i=0; i < game->playing; i++) {
        risk->players[i].armies *= 2;
        risk->players[i].armies += STARTING_ARMIES[game->playing];
        player_cmd_a(s, risk->players[i].player, SR_CMD_GET_ARMIES, 
            risk->players[i].armies);
    }
}

void give_game_status(GameInstance *g, const Server *s, Player *p) {
    SpeedRiskData *risk = (SpeedRiskData*)g->data;
    if (p == NULL) {
        s->tell_all(g, (char*)&risk->status,sizeof(SR_Game_Status));
    }
    else {
        s->tell_player(p, (char*)&risk->status,sizeof(SR_Game_Status));
    }
}

void give_country_status(GameInstance *g, const Server *s, Player *p, int country) {
    SpeedRiskData *risk = (SpeedRiskData*)g->data;
    SR_Country_Status status;
    status.command.command = SR_CMD_COUNTRY_STATUS;
    status.country.country = country;
    status.country.owner   = risk->status.countries[country].owner;
    status.country.armies  = risk->status.countries[country].armies;
    if (p == NULL) {
        s->tell_all(g, (char*)&status,sizeof(SR_Country_Status));
    }
    else {
        s->tell_player(p, (char*)&status,sizeof(SR_Country_Status));
    }
}

void game_init (GameInstance *g, const Server *s) {
    SpeedRiskData *risk = (SpeedRiskData*)malloc(sizeof(SpeedRiskData));
    bzero(risk, sizeof(SpeedRiskData));
    g->data = risk;
    risk->status.command.command = SR_CMD_GAME_STATUS;
    risk->army_generation_period = risk->board->army_generation_period;
    msg_error.command = SR_CMD_ERROR;
    msg_move.command.command = SR_CMD_MOVE_RESULT;
	g->state = &SR_WAITING_FOR_PLAYERS;
	s->log(g, "Initializing");
	sprintf(g->status, "Waiting for players");
}

void player_join (GameInstance *g, const Server *s, Player *p) {
    int id;
    SpeedRiskData *risk;
    risk = (SpeedRiskData*)g->data;
	for(id=0; id<risk->board->max_players; id++) {
		if (risk->players[id].player == NULL) {
			p->in_game_id = id;
			break;
		}
	}
	risk->players[p->in_game_id].player = p;
	risk->players[p->in_game_id].ready = false;
	risk->players[p->in_game_id].armies = 0;
	
	all_cmd_f(g, s, SR_CMD_PLAYER_JOIN, p->in_game_id);
	s->log(g, "Welcoming %s as player %i", p->name, p->in_game_id);
	if (g->playing == risk->board->max_players) {
		s->change_state(g, &SR_PLACING);
	}
}

void player_quit (GameInstance *g, const Server *s, Player *p) {
	int all_ready, i;
    SpeedRiskData *risk = (SpeedRiskData*)g->data;
	if (g->playing == 0) {
		s->game_over(g);
		return;
	}
    all_cmd_f(g, s, SR_CMD_PLAYER_QUIT, p->in_game_id);
    risk->players[p->in_game_id].player = NULL;
	if (g->playing == 1) {
		if (g->state == &SR_PLACING || g->state == &SR_RUNNING) {
			s->change_state(g, &SR_DONE);
		}
		return;
	}
	if (g->state == &SR_RUNNING || g->state == &SR_DONE) return;
	all_ready = true;
	for (i=0; i<risk->board->max_players; i++) {
		all_ready &= (risk->players[i].player == NULL || risk->players[i].ready);
	}
	if (all_ready) {
		if (g->state == &SR_WAITING_FOR_PLAYERS) {
			s->change_state(g, &SR_PLACING);
		}
		else {
			s->change_state(g, &SR_RUNNING);
		}
	}
}

void start_placing(GameInstance *g, const Server *s) {
	g->accepting_players = false;
	msg_command.command = SR_CMD_START_PLACING;
	s->tell_all(g, (char*)&msg_command, 4);
	init_board(g, s);
	give_game_status(g, s, NULL);
	strcpy(g->status, "Placing armies");
}

void start_playing(GameInstance *g, const Server *s) {
	msg_command.command = SR_CMD_BEGIN;
	s->tell_all(g,(char*)&msg_command,4);
	s->add_timer(g, ((SpeedRiskData*)g->data)->army_generation_period * 1000, false);
	s->log(g, "Starting SpeedRisk");
	strcpy(g->status, "At war");
	all_cmd_f(g, s, SR_CMD_NEXT_ARMY_PRODUCTION, ((SpeedRiskData*)g->data)->army_generation_period);
}

void start_ending(GameInstance *g, const Server *s) {
	SpeedRiskData *risk = (SpeedRiskData*)g->data;
	char *name;
	int p;
	int victor = 0;
	bool won = false;
	for (p=0; p<risk->board->max_players; p++) {
		if (risk->players[p].player != NULL) {
			victor = p;
			if (risk->players[p].countries_held == 42) {
				won = true;
				break;
			}
		}
	}
	name = risk->players[victor].player->name;
	all_cmd_f(g, s, SR_CMD_VICTORY, victor);
	if (won) {
		s->log(g, "%s victorious", name);
		snprintf(g->status, STATUS_LENGTH, "Game over: %s rules the world", name);
	}
	else {
		s->log(g, "%s stands alone", name);
		snprintf(g->status, STATUS_LENGTH, "Game over: %s has no opposition", name);
	}
	/* After 60 seconds, we'll want to boot everyone and exit the game */
	s->add_timer(g, 60000, false);
}

void quit_game (GameInstance *g, const Server *s) {
	s->game_over(g);
}

#define return_if_invalid(cmd) \
    if (    cmd->to >= SR_NUM_COUNRIES || \
          cmd->from >= SR_NUM_COUNRIES ) { \
        player_error(s, p, SR_ERR_INVALID_DESTINATION); \
        return; \
    }

void give_player_status(GameInstance *g, const Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *risk;
	int i;
	SR_Player *player;
    risk = (SpeedRiskData*)g->data;
	msg_command.command = SR_CMD_PLAYER_STATUS;
	msg_command.from = p->in_game_id;
	msg_command.to = g->playing;
	msg_command.armies = risk->players[p->in_game_id].armies;
	s->tell_player(p,(char*)&msg_command,4);

	for (i=0; i<risk->board->max_players; i++) {
		player = &risk->players[i];
		if (player->player != NULL) {
			player_cmd_f(s, p, player->ready ? SR_CMD_READY : SR_CMD_NOTREADY, i);
		}
	}
}

void handle_waiting(GameInstance *g, const Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *risk;
	int all_ready, i;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    risk = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_READY:
			if (g->playing > 1) {
				if (risk->players[p->in_game_id].ready)
					return;
				risk->players[p->in_game_id].ready = true;
				all_cmd_f(g, s, SR_CMD_READY, p->in_game_id);
				all_ready = true;
				for (i=0; i<risk->board->max_players; i++) {
					all_ready &= (risk->players[i].player == NULL || risk->players[i].ready);
				}
				if (all_ready) {
					s->change_state(g, &SR_PLACING);
				}
			}
			else {
				player_error(s, p, SR_ERR_NOT_ENOUGH_PLAYERS);
			}
			break;
		case SR_CMD_NOTREADY:
			if (!risk->players[p->in_game_id].ready)
				return;
			risk->players[p->in_game_id].ready = false;
			all_cmd_f(g, s, SR_CMD_NOTREADY, p->in_game_id);
			break;
		case SR_CMD_PLAYER_STATUS:
			give_player_status(g, s, p, req, len);
			break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void handle_placing(GameInstance *g, const Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *risk;
	int all_ready, i;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    risk = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_READY:
			if (risk->players[p->in_game_id].ready)
				return;
			risk->players[p->in_game_id].ready = true;
			all_cmd_f(g, s, SR_CMD_READY, p->in_game_id);
			all_ready = true;
			for (i=0; i<risk->board->max_players; i++) {
				all_ready &= (risk->players[i].player == NULL || risk->players[i].ready);
			}
			if (all_ready) {
				s->change_state(g, &SR_RUNNING);
			}
			break;
		case SR_CMD_NOTREADY:
			if (!risk->players[p->in_game_id].ready)
				return;
			risk->players[p->in_game_id].ready = false;
			all_cmd_f(g, s, SR_CMD_NOTREADY, p->in_game_id);
			break;
		case SR_CMD_PLACE:
			if (not_holds((&risk->status), p, cmd->to)) {
				player_error(s, p, SR_ERR_NOT_OWNER);
			}
			else if (cmd->armies > 
				risk->players[p->in_game_id].armies) {
				player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
			}
			else {
				risk->status.countries[cmd->to].armies += cmd->armies;
				risk->players[p->in_game_id].armies -= cmd ->armies;
				give_country_status(g, s, NULL, cmd->to);
				player_cmd_a(s, p, SR_CMD_GET_ARMIES,
					risk->players[p->in_game_id].armies);
			}
		case SR_CMD_PLAYER_STATUS:
			give_player_status(g, s, p, req, len);
            break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void do_move(GameInstance *g, const Server *s, Player *p, SR_Command *cmd) {
	SpeedRiskData *risk = (SpeedRiskData *)g->data;
	SR_Game_Status *status = &risk->status;
	if (!(holds(status, p, cmd->from) && holds(status, p, cmd->to))) {
		player_error(s, p, SR_ERR_NOT_OWNER);
	}
	else if (!borders(cmd->from, cmd->to)) {
		player_error(s, p, SR_ERR_INVALID_DESTINATION);
	}
	else if (cmd->armies>=status->countries[cmd->from].armies){
		player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
	}
	else {
		status->countries[cmd->from].armies -= cmd->armies;
		status->countries[cmd->to].armies   += cmd->armies;
		tell_all_mv_at_result(g, s, SR_CMD_MOVE_RESULT,
			cmd->from, cmd->to);
	}
}

void roll_for_attack (int attacking, int defending, long (*rand_func)(long max), int *attack_loss, int *defend_loss) {
    int red[3], white[2], i;
	bzero(&red,   3*sizeof(int));
	bzero(&white, 2*sizeof(int));
	for(i=0; i<attacking; i++) {
		red[i] = rand_func(6) + 1;
	}
	if (defending > attacking) defending = attacking;
	for(i=0; i<defending; i++) {
		white[i] = rand_func(6) + 1;
	}
	cmp_and_swap(red, 0, 1, i);
	cmp_and_swap(red, 0, 2, i);
	cmp_and_swap(red, 1, 2, i);
	cmp_and_swap(white, 0, 1, i);
	*attack_loss = *defend_loss = 0;
	for (i=0; i < defending; i++) {
		if (red[i] > white[i]) {
			(*defend_loss)++;
		} else {
			(*attack_loss)++;
		}
	}
}

void do_attack(GameInstance *g, const Server *s, Player *p, SR_Command *cmd) {
	SpeedRiskData *risk = (SpeedRiskData *)g->data;
    SR_Game_Status *status = &risk->status;
	int attacking, defending, attack_loss, defend_loss, defender;
	if (not_holds(status, p, cmd->from)) {
		player_error(s, p, SR_ERR_NOT_OWNER);
	}
	else if (holds(status, p, cmd->to)) {
		player_error(s, p, SR_ERR_INVALID_DESTINATION);
	}
	else if (!borders(cmd->from, cmd->to)) {
		player_error(s, p, SR_ERR_INVALID_DESTINATION);
	}
	else if (cmd->armies < 1 || cmd->armies >= 
		status->countries[cmd->from].armies) {
		player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
	}
	else {
		attacking = cmd->armies > 3 ? 3 : cmd->armies;
		defending = status->countries[cmd->to].armies > 1 ? 2 : 1;
		roll_for_attack(attacking, defending, s->random, &attack_loss, &defend_loss);
		status->countries[cmd->from].armies -= attack_loss;
		status->countries[cmd->to].armies   -= defend_loss;
		defender = status->countries[cmd->to].owner;
		if (status->countries[cmd->to].armies == 0) {
			attacking = cmd->armies - attack_loss;
			status->countries[cmd->from].armies -= attacking;
			status->countries[cmd->to].armies += attacking;
			status->countries[cmd->to].owner = p->in_game_id;
			risk->players[p->in_game_id].countries_held++;
			risk->players[defender].countries_held--;
		}
		tell_all_mv_at_result(g, s, SR_CMD_ATTACK_RESULT, cmd->from, cmd->to);
		if (risk->players[defender].countries_held == 0) {
			all_cmd_f(g, s, SR_CMD_DEFEAT, defender);
			if (risk->players[defender].player != NULL) {
				s->log(g, "%s defeated", risk->players[defender].player->name);
			}
		}
		if (risk->players[p->in_game_id].countries_held == 42) {
			s->change_state(g, &SR_DONE);
		}
	}
}

void handle_playing(GameInstance *g, const Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *risk;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    risk = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_MOVE:
			do_move(g, s, p, cmd);
			break;
		case SR_CMD_ATTACK:
			do_attack(g, s, p, cmd);
			break;
		case SR_CMD_PLACE:
			if (not_holds((&risk->status), p, cmd->to)) {
				player_error(s, p, SR_ERR_NOT_OWNER);
			}
			else if (cmd->armies > risk->players[p->in_game_id].armies) {
				player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
			}
			else {
				risk->status.countries[cmd->to].armies += cmd->armies;
				risk->players[p->in_game_id].armies -= cmd->armies;
				give_country_status(g, s, NULL, cmd->to);
				player_cmd_a(s, p, SR_CMD_GET_ARMIES,
					risk->players[p->in_game_id].armies);
			}
			break;
		case SR_CMD_GAME_STATUS:
			give_game_status(g, s, p);
			break;
		case SR_CMD_PLAYER_STATUS:
			msg_command.command = SR_CMD_PLAYER_STATUS;
			msg_command.from = p->in_game_id;
			msg_command.to = g->playing;
			msg_command.armies = risk->players[p->in_game_id].armies;
			s->tell_player(p,(char*)&msg_command,4);
			break;
		case SR_CMD_COUNTRY_STATUS:
			give_country_status(g, s, p, cmd->from);
			break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void handle_done (GameInstance *g, const Server *s, Player *p, const char *req, int len) {
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
	switch (cmd->command) {
		case SR_CMD_GAME_STATUS:
			give_game_status(g, s, p);
			break;
		case SR_CMD_COUNTRY_STATUS:
			give_country_status(g, s, p, cmd->from);
			break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

