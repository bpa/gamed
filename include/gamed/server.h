#ifndef GAMED_SERVER_H
#define GAMED_SERVER_H

#define GAMED_PORT 7483
#include <sys/types.h>
#include <event.h>
#include <gamed/game.h>
#include <gamed/player.h>

typedef struct st_client{
    Player player;
    Game *game;
    event ev;
} Client;

namespace Gamed {

class Server {
public:
    Server();
    void run_as_daemon();
    void run();
    void listen_on_port(int port);
};

}
#endif
