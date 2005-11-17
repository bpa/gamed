#ifndef GAMED_SERVER_H
#define GAMED_SERVER_H

#define GAMED_PORT 7483

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
