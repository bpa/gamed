#ifndef SERVER_H
#define SERVER_H

#include <sys/cdefs.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <event2/event.h>
#include <gamed/game.h>
#include "command.h"

#define GAMED_PORT 7483

typedef struct st_client_list_head ClientList;
typedef struct st_game_module_instance GameModuleInstance;
typedef struct st_game_module_instance_head GameModuleInstanceList;
typedef struct st_game_module_list_head GameModuleList;
LIST_HEAD(st_client_list_head, st_client);
LIST_HEAD(st_game_module_instance_head, st_game_module_instance);
LIST_HEAD(st_game_module_list_head, st_game_module);

typedef struct st_client {
	Player player;
	int sock;
	GameModuleInstance *game;
	LIST_ENTRY(st_client) client_entry;
	LIST_ENTRY(st_client) player_entry;
	struct event *ev;
} Client;

typedef struct st_game_module {
	Game game;
	LIST_ENTRY(st_game_module) game_entry;
	GameModuleInstanceList instance_list;
	int instances;
} GameModule;

struct st_game_module_instance {
	GameInstance instance;
	LIST_ENTRY(st_game_module_instance) game_instance_entry;
	ClientList players;
	GameModule *module;
	struct event *timer;
	struct timeval period;
	int timer_is_persistent;
};

typedef void (*command_func)(Client *, int);

extern char rand_state[8];
extern char buff[1024];
extern struct event_base *ev_base;

__BEGIN_DECLS

void init_server(int port, const char *config_file);
void run_server();
void run_as_daemon();
void handle_command(Client *, int len);
void read_config (const char *config);
void drop_client(Client *client, int r);

__END_DECLS

#endif
