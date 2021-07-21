#ifndef COMMAND_H
#define COMMAND_H

enum GAMED_COMMAND {
    CMD_NOP = 0,
    CMD_INVALID,
	CMD_ERROR,
	CMD_CHAT,
	CMD_PLAYER,
	CMD_GAME,
	CMD_ADMIN,
	CMD_COMMAND_SENTINEL
};

enum GAMED_ERRORS {
	GAMED_ERR_GAME_FULL = 0,
	GAMED_ERR_NO_GAME,
	GAMED_ERR_IN_GAME,
	GAMED_ERR_GAME_EXISTS
};

enum GAMED_SUBCMD_CHAT {
	CMD_TALK = 0,
	CMD_WHISPER,
	CMD_EMOTE,
	CMD_ADD_FRIEND,
	CMD_FRIEND_STATUS,
	CMD_REMOVE_FRIEND,
	CMD_CHAT_SENTINEL
};

enum GAMED_SUBCMD_PLAYER {
	CMD_RENAME = 0,
    CMD_PLAYER_JOIN,
    CMD_PLAYER_QUIT,
	CMD_PLAYER_SENTINEL
};

enum GAMED_SUBCMD_GAME {
	CMD_LIST_GAMES = 0,
	CMD_LIST_GAME_INSTANCES,
	CMD_CREATE_GAME,
	CMD_JOIN_GAME,
	CMD_LIST_PLAYERS,
	CMD_QUIT_GAME,
    CMD_MESSAGE,
	CMD_GAME_SENTINEL
};

typedef struct {
    unsigned command :  8;
    unsigned subcmd  :  8;
    unsigned length  : 16;
} GamedCommand;

/* I'm cheating because I use a standard convention of
 * cmd and client
 */
#define PLAYER_ERROR(s) \
	cmd->command = CMD_ERROR; \
	cmd->subcmd = s; \
	cmd->length = 0; \
    SEND_BUFF_TO_CLIENT(4)

#define CMD_PLAYER(c, s) \
	cmd->command = c; \
	cmd->subcmd = s; \
	cmd->length = 0; \
    SEND_BUFF_TO_CLIENT(4)

#define CMD_ALL_LEN(c, s, l) \
	cmd->command = c; \
	cmd->subcmd = s; \
	cmd->length = htons(l); \
    SEND_BUFF_TO_CLIENT(l+4)

#define SET_CMD(c, s, l, buff) \
    buff[0] = c; \
    buff[1] = s; \
    buff[2] = (len & 0xFF00) >> 8; \
    buff[3] = len & 0xFF;

#define SEND_BUFF_TO_CLIENT(len) \
    if (send(client->sock, &buff[0], len, MSG_NOSIGNAL) == -1) { \
		drop_client(client, -1); \
    }

#endif
