#ifndef GAMED_CHAT_H
#define GAMED_CHAT_H

#include <gamed/queue.h>
#include <gamed/event.h>
#include <gamed/player.h>

__BEGIN_DECLS

typedef struct st_chat_instance ChatInstance;

typedef struct st_chat {
	char name[32];
	char version[32];
	void (*initialize) (ChatInstance *c, Server *s);
	bool (*player_join)(ChatInstance *c, Server *s, Player *p);
    void (*player_chat)(ChatInstance *c, Server *s, Player *p, char *, int len);
	void (*player_quit)(ChatInstance *c, Server *s, Player *p);
	void (*destroy)    (ChatInstance *c, Server *s);
    LIST_ENTRY(st_chat) chat;
	/* TODO: Add config member */
} Chat;

struct st_chat_instance {
    char room[32];
    LIST_ENTRY(st_chat_instance) chat_instance;
    PlayerList players;
};

__END_DECLS

#endif
