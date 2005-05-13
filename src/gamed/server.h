#ifndef GAMED_SERVER_H
#define GAMED_SERVER_H

#include <gamed/config.h>
#define GAMED_PORT 7483

class GamedServer {
public:
	void daemonize();
	void start();
	Configuration config;
private:
	void listen_on_port(int port);
	void handle_connect(int sock, short event, void *args);
	void handle_network_event(int sock, short event, void *args);
};

#endif
