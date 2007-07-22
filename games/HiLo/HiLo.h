#ifndef GAME_HILO_H
#define GAME_HILO_H

__BEGIN_DECLS

typedef struct {
    int number;
    int guesses;
} HiLoData;

void game_init      (GameInstance *);
bool player_join    (GameInstance *, Player *);
void player_quit    (GameInstance *, Player *);

__END_DECLS

#endif
