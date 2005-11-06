#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <event.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sysexits.h>
//#include <perror.h>
#include <gamed/server.h>

void handle_sig_hup(int sock, short event, void *args);
void handle_connect(int sock, short event, void *args);
void handle_network_event(int sock, short event, void *args);
struct event ev_connection;
struct event ev_sig_hup;

/******************************************************************************/

Gamed::Server::Server() {
  event_init();
  signal(SIGPIPE,SIG_IGN);
  event_set(&ev_sig_hup, SIGHUP, EV_SIGNAL, handle_sig_hup, NULL);
      event_add(&ev_sig_hup,NULL);
  //lt_dlinit();
  //initstate(time(NULL),&random_state[0],32);
  //setstate(&random_state[0]);
}

/******************************************************************************/

void Gamed::Server::run_as_daemon() {
  if (daemon(1, 0) == -1) {
    perror("daemonize");
    //return EX_OSERR;
  }
  run();
}

/******************************************************************************/

void Gamed::Server::run() {
  listen_on_port(GAMED_PORT);
  event_dispatch();
}

/******************************************************************************/

void Gamed::Server::listen_on_port(int port) {
    int listener;
    struct sockaddr_in sa;
    listener = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listener < 0) {
        perror("socket");
        exit(0);
    }
    socklen_t sa_len = sizeof(sa);
    memset(&sa,0,sa_len);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (sockaddr*)&sa, sa_len) < 0) {
        perror("Unable to bind");
        exit(0);
    }
    if (listen(listener,5) < 0) {
        perror("Listen failed: ");
        exit(0);
    }
    event_set(&ev_connection, listener, EV_READ | EV_PERSIST,
         handle_connect, NULL);
    event_add(&ev_connection,NULL);
}

/******************************************************************************/

void handle_connect(int listener, short event, void *args) {
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);
    int sock = accept(listener, (struct sockaddr*)&sa, &sa_len);
    if (sock < 0) {
        perror("Failed connection");
    }
    printf("Got connection\n");
/*
    char ip_str[16];
    inet_ntop(AF_INET,&sa.sin_addr, ip_str, 15);
    info << "Recieved connection from " << ip_str << endl;
    GamedPlayer *pl = new GamedPlayer(sock);
    initialize_new_client(pl);
    event_set((struct event*)&pl->ev, sock, EV_READ | EV_PERSIST,
        &handle_network_event, pl);
    event_add((struct event*)&pl->ev,NULL);
*/
}

/******************************************************************************/

void handle_network_event(int sock, short event, void *args) {
}
/******************************************************************************/

void handle_sig_hup(int sock, short event, void *args) {
}
