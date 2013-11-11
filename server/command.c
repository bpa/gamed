#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

extern Server server_funcs;
extern GameModuleList game_module_list;

static void cmd_rename_player (Client *, int);
static void cmd_invalid       (Client *, int);

command_func player_commands[] = {
	cmd_rename_player,
    cmd_invalid,
    cmd_invalid
};

static void cmd_list_games     (Client *, int);
static void cmd_list_instances (Client *, int);
static void cmd_create_game    (Client *, int);
static void cmd_join_game      (Client *, int);
static void cmd_list_players   (Client *, int);
static void cmd_quit_game      (Client *, int);
static void cmd_game_message   (Client *, int);

command_func game_commands[] = {
	cmd_list_games,
	cmd_list_instances,
	cmd_create_game,
	cmd_join_game,
	cmd_list_players,
	cmd_quit_game,
    cmd_game_message
};

/******************************************************************************/
void cmd_invalid (Client *client, int len) {
	GamedCommand *cmd = (GamedCommand *)&buff[0];
    CMD_PLAYER(CMD_INVALID, 0);
}

/******************************************************************************/
void cmd_rename_player (Client *client, int len) {
	int buff_len = 4;
	char *data = &buff[4];
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	strncpy(client->player.name, data, 16);
	client->player.name[15] = '\0';
	if (len < 20) client->player.name[len-4] = '\0';
	if (client->game != NULL) {
		buff_len += sprintf(data, "%i:%s", client->player.in_game_id, &client->player.name[0]);
		CMD_ALL_LEN(CMD_RENAME, 0, buff_len);
	}
}

/******************************************************************************/
void cmd_list_games (Client *client, int len) {
	GameModule *module;
	int buff_len = 4;
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	LIST_FOREACH(module, &game_module_list, game_entry) {
		buff_len += sprintf(&buff[buff_len], "%s:%s:%i\n",
			module->game.name, module->game.version, module->instances);
	}
	buff_len--;
	buff[buff_len] = '\0';
	cmd->length = htons(buff_len-4);
    SEND_BUFF_TO_CLIENT(buff_len);
}

/******************************************************************************/
void cmd_list_instances (Client *client, int len) {
	/* TODO: Figuire out how to deal with it when there are more games than will fit in a K */
	/* XXX: if there are too many games, this will cause a core dump */
	int buff_len = 4;
	GameModule *module = NULL;
	GameModuleInstance *instance;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	LIST_FOREACH(module, &game_module_list, game_entry) {
		if (strncmp(module->game.name, &buff[4], 32) == 0) break;
	}
	if (module == NULL) {
		CMD_PLAYER(CMD_INVALID, 0);
		return;
	}
	buff_len += sprintf(&buff[buff_len], "%i", module->instances);
	LIST_FOREACH(instance, &module->instance_list, game_instance_entry) {
		buff_len += sprintf(&buff[buff_len], ":%s", instance->instance.name);
	}
	cmd->length = htons(buff_len-4);
    SEND_BUFF_TO_CLIENT(buff_len);
}

/******************************************************************************/
void cmd_create_game (Client *client, int len) {
	char *game_name, *instance_name;
	GameModule *module = NULL;
	GameModuleInstance *instance;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	if (client->game != NULL) {
		PLAYER_ERROR(GAMED_ERR_IN_GAME);
		return;
	}
	game_name = strtok(&buff[4], ":");
	instance_name = strtok(NULL, ":");
	if (game_name == NULL     || strlen(game_name) < 1 ||
		instance_name == NULL || strlen(instance_name) < 1) {
		CMD_PLAYER(CMD_INVALID, 0);
		return;
	}
	LIST_FOREACH(module, &game_module_list, game_entry) {
		if (strncmp(module->game.name, &buff[4], 32) == 0) break;
	}
	if (module == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
		return;
	}
	instance = LIST_FIRST(&module->instance_list);
	while (instance != NULL) {
		if (strncmp(instance->instance.name, instance_name, 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance_entry);
	}
	if (instance != NULL) {
		PLAYER_ERROR(GAMED_ERR_GAME_EXISTS);
        return;
	}
	module->instances++;
	instance = (void*)calloc(sizeof(GameModuleInstance), 1);
	strncpy(instance->instance.name, instance_name, 32);
	/* TODO: clean up names passed in */
	instance->instance.name[31] = '\0';
	instance->instance.accepting_players = true;
	instance->module = module;
	LIST_INSERT_HEAD(&module->instance_list, instance, game_instance_entry);
	LIST_INIT(&instance->players);
	LIST_INSERT_HEAD(&instance->players, client, player_entry);
	module->game.create((GameInstance*)instance, &server_funcs);
	client->game = instance;
	instance->instance.playing = 1;
	cmd->command = CMD_GAME;
	cmd->subcmd = CMD_CREATE_GAME;
	cmd->length = 0;
    SEND_BUFF_TO_CLIENT(4);
	server_funcs.log((GameInstance*)instance, "%s joined", &client->player.name[0]);
	module->game.player_join((GameInstance*)instance, &server_funcs, &client->player);
}

/******************************************************************************/
void cmd_join_game (Client *client, int len) {
	GameModule *module;
	GameModuleInstance *instance;
	char *game_name, *instance_name;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	game_name = strtok(&buff[4], ":");
	instance_name = strtok(NULL, ":");
	if (game_name == NULL     || strlen(game_name) < 1 ||
		instance_name == NULL || strlen(instance_name) < 1) {
		CMD_PLAYER(CMD_INVALID, 0);
		return;
	}
	LIST_FOREACH(module, &game_module_list, game_entry) {
		if (strncmp(module->game.name, &buff[4], 32) == 0) break;
	}
	if (module == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
		return;
	}
	instance = LIST_FIRST(&module->instance_list);
	while (instance != NULL) {
		if (strncmp(instance->instance.name, instance_name, 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance_entry);
	}
	if (instance != NULL) {
		if (instance->instance.accepting_players) {
			client->game = instance;
			instance->instance.playing++;
			LIST_INSERT_HEAD(&instance->players, client, player_entry);
			cmd->length = 0;
            SEND_BUFF_TO_CLIENT(4);
			server_funcs.log((GameInstance*)instance, "%s joined", client->player.name);
			instance->module->game.player_join((GameInstance*)instance, &server_funcs, (Player*)client);
		}
		else {
			PLAYER_ERROR(GAMED_ERR_GAME_FULL);
		}
	}
	else {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
	}
}

/******************************************************************************/
void cmd_list_players (Client *client, int len) {
	Client *plr;
	int buff_len = 4;
	int length;
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	if (client->game != NULL) {
		buff[buff_len++] = T_ARRAY;
		buff[buff_len++] = client->game->instance.playing;
	    LIST_FOREACH(plr, &client->game->players, player_entry) {
			buff[buff_len++] = T_ARRAY;
			buff[buff_len++] = 3;
			buff[buff_len++] = T_BYTE;
			buff[buff_len++] = plr->player.in_game_id;
			length = strlen(plr->player.name);
			buff[buff_len++] = T_STRING;
			buff[buff_len++] = length;
	        memcpy(&buff[buff_len], plr->player.name, length-1);
			buff_len += length;
			memcpy(&buff[buff_len], plr->player.data, plr->player.data_len);
			buff_len += plr->player.data_len;
	    }
	    cmd->length = htons(buff_len-4);
        SEND_BUFF_TO_CLIENT(buff_len);
	}
	else {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
	}
}

/******************************************************************************/
void cmd_quit_game (Client *client, int len) {
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	if (client->game == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
	}
	else {
		client->game->instance.playing--;
		LIST_REMOVE(client, player_entry);
		client->game->module->game.player_quit(
			(GameInstance*)client->game, &server_funcs, (Player*)client);
		cmd->length = 0;
        SEND_BUFF_TO_CLIENT(4);
		client->game = NULL;
	}
}

/******************************************************************************/
void cmd_game_message (Client *client, int len) {
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	if (client->game == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
	}
    else {
        client->game->instance.state->player_event(
            (GameInstance*)client->game, &server_funcs, (Player*)client, &buff[4], len-4);
    }
}

