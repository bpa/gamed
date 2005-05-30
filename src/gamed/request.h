#ifndef GAMED_REQUEST_H
#define GAMED_REQUEST_H

#include <gamed/player.h>

namespace Gamed {

class Request {
protected:
    Player *requestor;
};

class JoinRequest: public Request {
};

}
#endif
