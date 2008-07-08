#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/types.h>
#include <gamed/command.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "server.h"

extern Server server_funcs;
extern GameList game_list;
extern GameInstanceList game_instances;
extern ChatInstanceList chat_list;

static void cmd_identify_client(Client *, int);
static void cmd_rename_player  (Client *, int);

command_func player_commands[] = {
	cmd_identify_client,
	cmd_rename_player,
};

static void cmd_list_games     (Client *, int);
static void cmd_list_instances (Client *, int);
static void cmd_create_game    (Client *, int);
static void cmd_join_game      (Client *, int);
static void cmd_list_players   (Client *, int);
static void cmd_quit_game      (Client *, int);

command_func game_commands[] = {
	cmd_list_games,
	cmd_list_instances,
	cmd_create_game,
	cmd_join_game,
	cmd_list_players,
	cmd_quit_game,
};

/******************************************************************************/
void cmd_identify_client (Client *client, int len) {
	/* TODO: Figure out how this would actually be used or delete it */
}

/******************************************************************************/
void cmd_rename_player (Client *client, int len) {
	int buff_len = 0;
	char *data = &buff[4];
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	strncpy(client->player.name, data, 16);
	client->player.name[15] = '\0';
	if (len < 20) client->player.name[len-4] = '\0';
	if (client->game != NULL) {
		buff_len = sprintf(data, "%i:%s", client->player.in_game_id, &client->player.name[0]);
		cmd->command = CMD_RENAME;
		cmd->subcmd = 0;
		cmd->length = htons(buff_len);
		server_funcs.tell_all(client->game, &buff[0], buff_len+4);
	}
}

/******************************************************************************/
void cmd_list_games (Client *client, int len) {
	Game *game;
	int buff_len = 4;
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	LIST_FOREACH(game, &game_list, game) {
		buff_len += sprintf(&buff[buff_len], "%s %s\n", game->name, game->version);
	}
	buff_len--;
	buff[buff_len] = '\0';
	cmd->length = htons(buff_len);
	server_funcs.tell_player(&client->player, &buff[0], buff_len);
}

/******************************************************************************/
void cmd_list_instances (Client *client, int len) {
	/* TODO: Figuire out how to deal with it when there are more games than will fit in a K */
	/* TODO: Should we require you specify which game you are interested in?  This would require keeping
	 *       a list of game instances connected to the main game instance instead of in a combined list */
	/* XXX: if there are too many games, this will cause a core dump */
	int buff_len = 4;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	GameInstance *instance;
	LIST_FOREACH(instance, &game_instances, game_instance) {
		buff_len += sprintf(&buff[buff_len], "%s %s %s\n", instance->name, instance->game->name, instance->game->version);
	}
	buff_len--;
	buff[buff_len] = '\0';
	cmd->length = htons(buff_len);
	server_funcs.tell_player(&client->player, &buff[0], buff_len);
}

/******************************************************************************/
void cmd_create_game (Client *client, int len) {
	char *game_name, *instance_name;
	Game *game = NULL;
	GameInstance *instance;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	if (client->game != NULL) {
		cmd->command = CMD_ERROR;
		cmd->subcmd = GAMED_ERR_IN_GAME;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
		return;
	}
	game_name = strtok(&buff[4], ":");
	instance_name = strtok(NULL, ":");
	if (game_name == NULL     || strnlen(game_name,32) < 1 ||
		instance_name == NULL || strnlen(instance_name, 32) < 1) {
		cmd->command = CMD_INVALID;
		cmd->subcmd = 0;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
		return;
	}
	instance = LIST_FIRST(&game_instances);
	while (instance != NULL) {
		if (strncmp(instance->name, &buff[4], 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance);
	}
	if (instance != NULL) {
        cmd->command = CMD_ERROR;
        cmd->subcmd = GAMED_ERR_GAME_EXISTS;
        cmd->length = 0;
        server_funcs.tell_player(&client->player, &buff[0], 4);
        return;
	}
	LIST_FOREACH(game, &game_list, game) {
		if (strncmp(game->name, &buff[4], 32) == 0) break;
	}
	if (game != NULL) {
		game = game;
		instance = (void*)malloc(sizeof(GameInstance));
		memset(instance, 0, sizeof(GameInstance));
		strncpy(instance->name, instance_name, 32);
		/* TODO: clean up names passed in */
		instance->name[31] = '\0';
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
		instance->game = game;
		LIST_INSERT_HEAD(&game_instances, instance, game_instance);
		LIST_INIT(&instance->players);
		game->create(instance, &server_funcs);
		if (game->player_join(instance, &server_funcs, &client->player)) {
			syslog(LOG_INFO, "%s joined", client->player.name);
			client->game = instance;
		}
	}
	else {
		cmd->command = CMD_ERROR;
		cmd->subcmd = GAMED_ERR_NO_GAME;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
}

/******************************************************************************/
void cmd_join_game (Client *client, int len) {
	GameInstance *instance;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	buff[len] = '\0';
	instance = LIST_FIRST(&game_instances);
	while (instance != NULL) {
		if (strncmp(instance->name, &buff[4], 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance);
	}

	if (instance != NULL) {
		if (instance->game->player_join(instance, &server_funcs, &client->player)) {
			client->game = instance;
			syslog(LOG_INFO, "%s joined", client->player.name);
			cmd->length = 0;
			server_funcs.tell_player(&client->player, &buff[0], 4);
		}
		else {
			cmd->command = CMD_ERROR;
			cmd->subcmd = GAMED_ERR_GAME_FULL;
			cmd->length = 0;
			server_funcs.tell_player(&client->player, &buff[0], 4);
		}
	}
	else {
		cmd->command = CMD_ERROR;
		cmd->subcmd = GAMED_ERR_NO_GAME;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
}

/******************************************************************************/
void cmd_list_players (Client *client, int len) {
	Player *plr;
	int buff_len = 4;
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	if (client->game != NULL) {
		buff_len += sprintf(&buff[buff_len], "%i", client->game->playing);
	    LIST_FOREACH(plr, &client->game->players, player) {
	        buff_len += sprintf(&buff[buff_len], ":%i:%s",
	             plr->in_game_id, &plr->name[0]);
	    }
		buff[buff_len] = '\0';
	    cmd->length = htons(buff_len);
	    server_funcs.tell_player(&client->player, &buff[0], buff_len);
	}
	else {
		cmd->command = CMD_ERROR;
		cmd->subcmd = GAMED_ERR_NO_GAME;
		cmd->length = 0;
	    server_funcs.tell_player(&client->player, &buff[0], 4);
	}
}

/******************************************************************************/
void cmd_quit_game (Client *client, int len) {
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	if (client->game == NULL) {
		cmd->command = CMD_ERROR;
		cmd->subcmd = GAMED_ERR_NO_GAME;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
	else {
		client->game->game->player_quit(client->game, &server_funcs, &client->player);
		LIST_REMOVE(&client->player, player);
		client->game = NULL;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
}
