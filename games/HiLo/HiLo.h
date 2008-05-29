#ifndef GAME_HILO_H
#define GAME_HILO_H

#include <gamed/game.h>

__BEGIN_DECLS

extern Game HiLo;

typedef struct {
    int number;
    int guesses;
} HiLoData;

__END_DECLS

#endif
