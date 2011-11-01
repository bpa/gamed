#define GCRYPT_NO_MPI_MACROS 1
#define GCRYPT_NO_DEPRECATED 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <event2/event.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

EventQueue queue;
struct event_base *event_base;

void cb_func(evutil_socket_t fd, short what, void *arg) {
	const char *data = arg;
	printf("Got an event on socket %d:%s%s%s%s [%s]",
			(int) fd,
			(what&EV_TIMEOUT) ? " timeout" : "",
			(what&EV_READ)    ? " read" : "",
			(what&EV_WRITE)   ? " write" : "",
			(what&EV_SIGNAL)  ? " signal" : "",
			data);
}

static void on_connect(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data) {
	struct event *ev1 = event_new(event_base, fd, EV_READ|EV_PERSIST, cb_func, (char*)"Reading event");
	event_add(ev1, NULL);
}

int listen_on_port(int port, int listen_any) {
	struct evconnlistener *listener;
    struct sockaddr_in sin;

	TAILQ_INIT(&queue);

	event_base = event_base_new();
	if (!event_base) {
		puts("Couldn't open event base");
		exit(1);
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(listen_any ? INADDR_ANY : 0);
	sin.sin_port = htons(port);

	listener = evconnlistener_new_bind(event_base, on_connect, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&sin, sizeof(sin)); if (!listener) {
		perror("Couldn't create listener");
		exit(1);
	}

	return 0;
}

Event *next_event() {
	Event *ev;
	if (queue.tqh_first == NULL) {
		event_base_loop(event_base, EVLOOP_ONCE);
	}
	ev = queue.tqh_first;
	TAILQ_REMOVE(&queue, queue.tqh_first, events);
	return ev;
}

Client *c_event_client(Event *event) { return event->client; }
const unsigned char *c_event_message(Event *event) { return event->message; }
int c_event_type(Event *event) { return event->type; }
int c_event_free(Event *event) { free(event); return 0; }
