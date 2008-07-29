#ifndef COMMAND_H
#define COMMAND_H

/* I'm cheating because I use a standard convention of
 * cmd and client
 */
#define PLAYER_ERROR(s) \
	cmd->command = CMD_ERROR; \
	cmd->subcmd = s; \
	cmd->length = 0; \
	server_funcs.tell_player((Player*)client, &buff[0], 4);

#define CMD_PLAYER(c, s) \
	cmd->command = c; \
	cmd->subcmd = s; \
	cmd->length = 0; \
	server_funcs.tell_player((Player*)client, &buff[0], 4);

#define CMD_ALL_LEN(c, s) \
	cmd->command = c; \
	cmd->subcmd = s; \
	cmd->length = htons(buff_len); \
	server_funcs.tell_all((GameInstance*)client->game, &buff[0], buff_len+4);

#endif
