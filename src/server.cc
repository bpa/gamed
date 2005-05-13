#include <gamed/server.h>
#include <time.h>
#include <event.h>
#include <stdlib.h>
#include <sysexits.h>
#include <perror.h>
#include <iostream>

/******************************************************************************/

void GamedServer::daemonize() {
	if (daemon(1, 0) == -1) {
		perror("daemonize");
		return EX_OSERR;
	}
	start();
}

/******************************************************************************/

void GamedServer::start() {
	listen_on_port(GAMED_PORT);
}

/******************************************************************************/

void GamedServer::listen_on_port(port) {
    int listener;
    struct sockaddr_in sa;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        fatal << "socket" << strerror(errno) << endl;
        exit(0);
    }
    socklen_t sa_len = sizeof(sa);
    memset(&sa,0,sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(7654);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    info << "Binding to port " << 7654 << endl;
    if (bind(listener, (sockaddr*)&sa, sa_len) < 0) {
        fatal << "Unable to bind to port " << 7654 << ": ";
        fatal << strerror(errno) << endl;
        exit(0);
    }
    if (listen(listener,5) < 0) {
        fatal << "Listen failed: " << strerror(errno) << endl;
        exit(0);
    }
/*
    event_set(&ev_connection, listener, EV_READ | EV_PERSIST,
         handle_connect, NULL);
    event_add(&ev_connection,NULL);
*/
}

/******************************************************************************/

void GamedServer::handle_connect(int sock, short event, void *args) {
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    int sock = accept(listener, (struct sockaddr*)&sa, &sa_len);
    if (sock < 0) {
        cout << "Failed connection: " << strerror(errno) << endl;
    }
/*
    char ip_str[16];
    inet_ntop(AF_INET,&sa.sin_addr, ip_str, 15);
    info << "Recieved connection from " << ip_str << endl;
    GamedPlayer *pl = new GamedPlayer(sock);
    initialize_new_client(pl);
    event_set((struct event*)&pl->ev, sock, EV_READ | EV_PERSIST,
        &handle_network_event, pl);
    event_add((struct event*)&pl->ev,NULL);
}

/******************************************************************************/

void GamedServer::handle_network_event(int sock, short event, void *args) {
}
