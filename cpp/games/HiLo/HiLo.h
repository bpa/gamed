#ifndef GAME_HILO_H
#define GAME_HILO_H

#include <gamed/game.h>

__BEGIN_DECLS

extern Game HiLo;

typedef struct {
    int number;
    int guesses;
} HiLoData;

enum HILO_COMMANDS {
	HILO_GUESS = 0,
	HILO_GIVEUP,
	HILO_TOO_LOW,
	HILO_TOO_HIGH,
	HILO_CORRECT
};

__END_DECLS

#endif
