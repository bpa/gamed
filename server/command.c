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
#include "command.h"

extern Server server_funcs;
extern GameList game_list;
extern ChatInstanceList chat_list;

static void cmd_rename_player  (Client *, int);

command_func player_commands[] = {
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
void cmd_rename_player (Client *client, int len) {
	int buff_len = 4;
	char *data = &buff[4];
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	strncpy(client->player.name, data, 16);
	client->player.name[15] = '\0';
	if (len < 20) client->player.name[len-4] = '\0';
	if (client->game != NULL) {
		buff_len += sprintf(data, "%i:%s", client->player.in_game_id, &client->player.name[0]);
		CMD_ALL_LEN(CMD_RENAME, 0);
	}
}

/******************************************************************************/
void cmd_list_games (Client *client, int len) {
	Game *game;
	int buff_len = 4;
	GamedCommand *cmd = (GamedCommand *)&buff[0];
	LIST_FOREACH(game, &game_list, game) {
		buff_len += sprintf(&buff[buff_len], "%s:%s:%i\n", game->name, game->version, game->instances);
	}
	buff_len--;
	buff[buff_len] = '\0';
	cmd->length = htons(buff_len);
	server_funcs.tell_player(&client->player, &buff[0], buff_len);
}

/******************************************************************************/
void cmd_list_instances (Client *client, int len) {
	/* TODO: Figuire out how to deal with it when there are more games than will fit in a K */
	/* XXX: if there are too many games, this will cause a core dump */
	int buff_len = 4;
	Game *game = NULL;
	GameInstance *instance;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	LIST_FOREACH(game, &game_list, game) {
		if (strncmp(game->name, &buff[4], 32) == 0) break;
	}
	if (game == NULL) {
		CMD_PLAYER(CMD_INVALID, 0);
		return;
	}
	buff_len += sprintf(&buff[buff_len], "%i", game->instances);
	LIST_FOREACH(instance, &game->instance_list, game_instance) {
		buff_len += sprintf(&buff[buff_len], ":%s", instance->name);
	}
	cmd->length = htons(buff_len-4);
	server_funcs.tell_player(&client->player, &buff[0], buff_len);
}

/******************************************************************************/
void cmd_create_game (Client *client, int len) {
	char *game_name, *instance_name;
	Game *game = NULL;
	GameInstance *instance;
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
	LIST_FOREACH(game, &game_list, game) {
		if (strncmp(game->name, &buff[4], 32) == 0) break;
	}
	if (game == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
		return;
	}
	instance = LIST_FIRST(&game->instance_list);
	while (instance != NULL) {
		if (strncmp(instance->name, instance_name, 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance);
	}
	if (instance != NULL) {
		PLAYER_ERROR(GAMED_ERR_GAME_EXISTS);
        return;
	}
	game->instances++;
	instance = (void*)malloc(sizeof(GameInstance));
	memset(instance, 0, sizeof(GameInstance));
	strncpy(instance->name, instance_name, 32);
	/* TODO: clean up names passed in */
	instance->name[31] = '\0';
	instance->game = game;
	LIST_INSERT_HEAD(&game->instance_list, instance, game_instance);
	LIST_INIT(&instance->players);
	game->create(instance, &server_funcs);
	if (game->player_join(instance, &server_funcs, &client->player)) {
		syslog(LOG_INFO, "%s joined", client->player.name);
		client->game = instance;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
	else {
		/* I really don't expect this to ever happen, its here for completeness */
		PLAYER_ERROR(GAMED_ERR_GAME_FULL);
	}
}

/******************************************************************************/
void cmd_join_game (Client *client, int len) {
	Game *game;
	GameInstance *instance;
	char *game_name, *instance_name;
	GamedCommand *cmd = (GamedCommand*)&buff[0];
	game_name = strtok(&buff[4], ":");
	instance_name = strtok(NULL, ":");
	if (game_name == NULL     || strlen(game_name) < 1 ||
		instance_name == NULL || strlen(instance_name) < 1) {
		CMD_PLAYER(CMD_INVALID, 0);
		return;
	}
	LIST_FOREACH(game, &game_list, game) {
		if (strncmp(game->name, &buff[4], 32) == 0) break;
	}
	if (game == NULL) {
		PLAYER_ERROR(GAMED_ERR_NO_GAME);
		return;
	}
	instance = LIST_FIRST(&game->instance_list);
	while (instance != NULL) {
		if (strncmp(instance->name, instance_name, 32) == 0) {
			break;
		}
		instance = LIST_NEXT(instance, game_instance);
	}

	if (instance != NULL) {
		if (instance->game->player_join(instance, &server_funcs, &client->player)) {
			syslog(LOG_INFO, "%s joined", client->player.name);
			client->game = instance;
			cmd->length = 0;
			server_funcs.tell_player(&client->player, &buff[0], 4);
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
		client->game->game->player_quit(client->game, &server_funcs, &client->player);
		LIST_REMOVE(&client->player, player);
		client->game = NULL;
		cmd->length = 0;
		server_funcs.tell_player(&client->player, &buff[0], 4);
	}
}
