#ifndef GAMED_PLAYER_H
#define GAMED_PLAYER_H

#include <stddef.h>
#include <sys/queue.h>

LIST_HEAD(st_player_list_head, st_player);
typedef struct st_player_list_head PlayerList;

struct st_player {
    LIST_ENTRY(st_player) players;
    char name[16];
    int sock;
    int in_game_id;
};
typedef struct st_player Player;

#endif
