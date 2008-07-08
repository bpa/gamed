#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <gamed/game.h>
#include <gamed/queue.h>
#include <SpeedRisk/board.h>
#include <SpeedRisk/SpeedRisk.h>

static SR_Command msg_command;
/* static SR_Country msg_country; */
static SR_Error   msg_error;
static SR_Move_Result msg_move;
static int STARTING_ARMIES[] = { 0, 0, 26, 21, 19, 16, 13 };

void game_init     (GameInstance *g, Server *s);
bool player_join   (GameInstance *g, Server *s, Player *p);
void player_quit   (GameInstance *g, Server *s, Player *p);
void handle_waiting(GameInstance *g, Server *s, Player *p, const char *, int len);
void handle_placing(GameInstance *g, Server *s, Player *p, const char *, int len);
void handle_playing(GameInstance *g, Server *s, Player *p, const char *, int len);
void handle_done   (GameInstance *g, Server *s, Player *p, const char *, int len);

Game SpeedRisk = { STANDARD_GAMED_GAME, "SpeedRisk", "0.2", game_init, NULL, player_join, player_quit };
State SR_WAITING_FOR_PLAYERS = { NULL, handle_waiting, NULL,            NULL };
State SR_PLACING             = { NULL, handle_placing, NULL,            NULL };
State SR_RUNNING             = { NULL, handle_playing, produce_armies, NULL };
State SR_DONE                = { NULL, handle_done,    NULL,            NULL };

#define all_cmd_f(g, s, cmd, fro) \
    msg_command.command = cmd; \
    msg_command.from = fro; \
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
        if ((int)board->status.countries[c].owner != i) { \
             holds_continent = false; break; } \
    } \
    if (holds_continent) { armies += value; }

void produce_armies (GameInstance *g, Server *s) {
    bool holds_continent;
    int i, c, armies, held;
    SR_Player *p;
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    for (i=0; i<g->playing; i++) {
        p = &board->players[i];
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
}

void init_board(GameInstance *game, Server *s) {
    int c, i, armies, player, pass;
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
            i = s->random(SR_NUM_COUNRIES);
            player = status->countries[c].owner;
            status->countries[c].owner = status->countries[i].owner;
            status->countries[i].owner = player;
        }
    }
    for (i=0; i < game->playing; i++) {
        board->players[i].armies *= 2;
        board->players[i].armies += STARTING_ARMIES[game->playing];
        player_cmd_a(s, board->players[i].player, SR_CMD_GET_ARMIES, 
            board->players[i].armies);
    }
}

void give_game_status(GameInstance *g, Server *s, Player *p) {
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    if (p == NULL) {
        s->tell_all(g, (char*)&board->status,sizeof(SR_Game_Status));
    }
    else {
        s->tell_player(p, (char*)&board->status,sizeof(SR_Game_Status));
    }
}

void give_country_status(GameInstance *g, Server *s, Player *p, int country) {
    SpeedRiskData *board = (SpeedRiskData*)g->data;
    SR_Country_Status status;
    status.command.command = SR_CMD_COUNTRY_STATUS;
    status.country.country = country;
    status.country.owner   = board->status.countries[country].owner;
    status.country.armies  = board->status.countries[country].armies;
    if (p == NULL) {
        s->tell_all(g, (char*)&status,sizeof(SR_Country_Status));
    }
    else {
        s->tell_player(p, (char*)&status,sizeof(SR_Country_Status));
    }
}

void game_init (GameInstance *g, Server *s) {
    SpeedRiskData *data = (SpeedRiskData*)malloc(sizeof(SpeedRiskData));
    LIST_INIT(&(g->players));
    g->data = (char*)data;
    bzero(g->data, sizeof(SpeedRiskData));
    data->status.command.command = SR_CMD_GAME_STATUS;
    g->playing = 0;
    msg_error.command = SR_CMD_ERROR;
    msg_move.command.command = SR_CMD_MOVE_RESULT;
    build_border_table();
	g->state = &SR_WAITING_FOR_PLAYERS;
}

bool player_join (GameInstance *g, Server *s, Player *p) {
    int id;
    Player *plr;
    SR_Player *sr_player;
    SpeedRiskData *board;
    if (g->state != &SR_WAITING_FOR_PLAYERS) return false;
    LIST_FOREACH(plr, &g->players, player) {
        if (plr == p) return false;
    }
    board = (SpeedRiskData*)g->data;
	if (g->playing < SR_MAX_PLAYERS) {
		LIST_INSERT_HEAD(&g->players, p, player);
		for(id=0; id<SR_MAX_PLAYERS; id++) {
			if (board->players[id].player == NULL) {
				p->in_game_id = id;
				break;
			}
		}
		board->players[p->in_game_id].player = p;
		board->players[p->in_game_id].ready = false;
		board->players[p->in_game_id].armies = 0;
		g->playing++;
		
		all_cmd_f(g, s, SR_CMD_PLAYER_JOIN, p->in_game_id);
		LIST_FOREACH(plr, &g->players, player) {
			id = plr->in_game_id;
			sr_player = &board->players[id];
			if (sr_player->ready) {
				player_cmd_f(s, p, SR_CMD_READY, id);
			}
			else {
				player_cmd_f(s, p, SR_CMD_NOTREADY, id);
			}
		}
		return true;
	}
	player_error(s, p, SR_CMD_ERROR);
	return false;
}

void player_quit (GameInstance *g, Server *s, Player *p) {
	int all_ready, i;
    struct timeval period;
    SpeedRiskData *srd = (SpeedRiskData*)g->data;
    LIST_REMOVE(p, player);
    g->playing--;
	if (g->playing == 0) {
		s->game_over(g);
		return;
	}
    all_cmd_f(g, s, SR_CMD_PLAYER_QUIT, p->in_game_id);
    srd->players[p->in_game_id].player = NULL;
    if (g->state == &SR_RUNNING) {
        syslog(LOG_INFO, "%s dropped", p->name);
    }
	if (g->playing == 1) {
		if (g->state == &SR_PLACING || g->state == &SR_RUNNING) {
			all_cmd_f(g, s, SR_CMD_VICTORY, p->in_game_id);
			g->state = &SR_DONE;
			syslog(LOG_INFO, "%s victorious", p->name);
			closelog();
		}
		return;
	}
	if (g->state == &SR_RUNNING || g->state == &SR_DONE) return;
	all_ready = true;
	for (i=0; i<SR_MAX_PLAYERS; i++) {
		all_ready &= (srd->players[i].player == NULL || srd->players[i].ready);
	}
	if (all_ready) {
		/* Yes, this smells of duplicate code */
		if (g->state == &SR_WAITING_FOR_PLAYERS) {
			g->state = &SR_PLACING;
			msg_command.command = SR_CMD_START_PLACING;
			s->tell_all(g, (char*)&msg_command, 4);
			init_board(g, s);
			give_game_status(g, s, NULL);
		}
		else {
			g->state = &SR_RUNNING;
			msg_command.command = SR_CMD_BEGIN;
			s->tell_all(g,(char*)&msg_command,4);
			period.tv_sec = SR_GENERATION_PERIOD;
			period.tv_usec = 0;
			s->add_timer(g, &period, true);
			syslog(LOG_INFO, "Starting SpeedRisk");
		}
	}
}

#define return_if_invalid(cmd) \
    if (    cmd->to >= SR_NUM_COUNRIES || \
          cmd->from >= SR_NUM_COUNRIES ) { \
        player_error(s, p, SR_ERR_INVALID_DESTINATION); \
        return; \
    }

void handle_waiting(GameInstance *g, Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *srd;
	int all_ready, i;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    srd = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_READY:
			if (g->playing > 1) {
				srd->players[p->in_game_id].ready = true;
				all_cmd_f(g, s, SR_CMD_READY, p->in_game_id);
				all_ready = true;
				for (i=0; i<SR_MAX_PLAYERS; i++) {
					all_ready &= (srd->players[i].player == NULL || srd->players[i].ready);
				}
				if (all_ready) {
					g->state = &SR_PLACING;
					msg_command.command = SR_CMD_START_PLACING;
					s->tell_all(g, (char*)&msg_command, 4);
					init_board(g, s);
					give_game_status(g, s, NULL);
				}
			}
			else {
				player_error(s, p, SR_ERR_NOT_ENOUGH_PLAYERS);
			}
			break;
		case SR_CMD_NOTREADY:
			srd->players[p->in_game_id].ready = false;
			all_cmd_f(g, s, SR_CMD_NOTREADY, p->in_game_id);
			break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void handle_placing(GameInstance *g, Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *srd;
    struct timeval period;
	int all_ready, i;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    srd = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_READY:
			srd->players[p->in_game_id].ready = true;
			all_cmd_f(g, s, SR_CMD_READY, p->in_game_id);
			all_ready = true;
			for (i=0; i<SR_MAX_PLAYERS; i++) {
				all_ready &= (srd->players[i].player == NULL || srd->players[i].ready);
			}
			if (all_ready) {
				g->state = &SR_RUNNING;
				msg_command.command = SR_CMD_BEGIN;
				s->tell_all(g,(char*)&msg_command,4);
				period.tv_sec = SR_GENERATION_PERIOD;
				period.tv_usec = 0;
				s->add_timer(g, &period, true);
				syslog(LOG_INFO, "Starting SpeedRisk");
			}
			break;
		case SR_CMD_NOTREADY:
			srd->players[p->in_game_id].ready = false;
			all_cmd_f(g, s, SR_CMD_NOTREADY, p->in_game_id);
			break;
		case SR_CMD_PLACE:
			if (not_holds((&srd->status), p, cmd->to)) {
				player_error(s, p, SR_ERR_NOT_OWNER);
			}
			else if (cmd->armies > 
				srd->players[p->in_game_id].armies) {
				player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
			}
			else {
				srd->status.countries[cmd->to].armies += cmd->armies;
				srd->players[p->in_game_id].armies -= cmd ->armies;
				give_country_status(g, s, NULL, cmd->to);
				player_cmd_a(s, p, SR_CMD_GET_ARMIES,
					srd->players[p->in_game_id].armies);
			}
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void do_move(GameInstance *g, Server *s, Player *p, SR_Command *cmd) {
	SpeedRiskData *srd = (SpeedRiskData *)g->data;
	SR_Game_Status *status = &srd->status;
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

void do_attack(GameInstance *g, Server *s, Player *p, SR_Command *cmd) {
	SpeedRiskData *srd = (SpeedRiskData *)g->data;
    SR_Game_Status *status = &srd->status;
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
			srd->players[p->in_game_id].countries_held++;
			srd->players[defender].countries_held--;
		}
		tell_all_mv_at_result(g, s, SR_CMD_ATTACK_RESULT, cmd->from, cmd->to);
		if (srd->players[defender].countries_held == 0) {
			all_cmd_f(g, s, SR_CMD_DEFEAT, defender);
			if (srd->players[defender].player != NULL) {
				syslog(LOG_INFO, "%s defeated", srd->players[defender].player->name);
			}
		}
		if (srd->players[p->in_game_id].countries_held == 42) {
			all_cmd_f(g, s, SR_CMD_VICTORY, p->in_game_id);
			g->state = &SR_DONE;
			syslog(LOG_INFO, "%s victorious", p->name);
			closelog();
		}
	}
}

void handle_playing(GameInstance *g, Server *s, Player *p, const char *req, int len) {
    SpeedRiskData *srd;
    SR_Command *cmd = (SR_Command*)req;
	return_if_invalid(cmd);
    srd = (SpeedRiskData*)g->data;
	switch (cmd->command) {
		case SR_CMD_MOVE:
			do_move(g, s, p, cmd);
			break;
		case SR_CMD_ATTACK:
			do_attack(g, s, p, cmd);
			break;
		case SR_CMD_PLACE:
			if (not_holds((&srd->status), p, cmd->to)) {
				player_error(s, p, SR_ERR_NOT_OWNER);
			}
			else if (cmd->armies > srd->players[p->in_game_id].armies) {
				player_error(s, p, SR_ERR_NOT_ENOUGH_ARMIES);
			}
			else {
				srd->status.countries[cmd->to].armies += cmd->armies;
				srd->players[p->in_game_id].armies -= cmd->armies;
				give_country_status(g, s, NULL, cmd->to);
				player_cmd_a(s, p, SR_CMD_GET_ARMIES,
					srd->players[p->in_game_id].armies);
			}
			break;
		case SR_CMD_GAME_STATUS:
			give_game_status(g, s, p);
			break;
		case SR_CMD_PLAYER_STATUS:
			player_cmd_a(s, p, SR_CMD_PLAYER_STATUS, srd->players[p->in_game_id].armies);
			break;
		case SR_CMD_COUNTRY_STATUS:
			give_country_status(g, s, p, cmd->from);
			break;
		default:
			player_error(s, p, SR_ERR_INVALID_CMD);
			break;
	}
}

void handle_done (GameInstance *g, Server *s, Player *p, const char *req, int len) {
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

