#ifndef CLIENT_SUPPORTING_LIB
#define CLIENT_SUPPORTING_LIB
#include <sys/queue.h>

typedef struct st_client_list_head ClientList;
typedef struct st_event_queue_head EventQueue;
LIST_HEAD(st_client_list_head, st_client);
TAILQ_HEAD(st_event_queue_head, st_event);

typedef struct st_client {
	int sock;
    LIST_ENTRY(st_client) client_entry;
} Client;

typedef struct st_event {
	int type;
	unsigned char *message;
	Client *client;
	TAILQ_ENTRY(st_event) events;
} Event;

#endif
