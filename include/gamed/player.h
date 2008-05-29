#ifndef GAMED_PLAYER_H
#define GAMED_PLAYER_H

#include <gamed/queue.h>

__BEGIN_DECLS

typedef struct st_player {
    int sock;
    char name[16];
    int in_game_id;
    LIST_ENTRY(st_player) player;
} Player;

LIST_HEAD(st_player_list_head, st_player);
typedef struct st_player_list_head PlayerList;

__END_DECLS

#endif
