#ifndef GAME_HILO_H
#define GAME_HILO_H

#include <gamed/game.h>

class HiLo: public Gamed::Game {
public:
    HiLo();
    void handle_request(Gamed::Player *, Gamed::Request *);
private:
    int number;
    int guesses;
};

#endif
