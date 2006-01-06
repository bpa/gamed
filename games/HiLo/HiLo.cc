#include "HiLo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <gamed/game.h>
#include <gamed/utils.h>
#include <gamed/player.h>

char hilobuff[35];

void choose_number(HiLoData *data) {
        data->number = (random() % 100) + 1;
        data->guesses = 0;
}

void game_init (Game *g) {
    LIST_INIT(&(g->players));
    HiLoData *data = (HiLoData*)malloc(sizeof(HiLoData));
    g->data = (char*)data;
    choose_number(data);
}

bool player_join (Game *g, Player *p) {
    if (LIST_EMPTY(&g->players)) {
        LIST_INSERT_HEAD(&g->players, p, players);
        send(p->sock, "Welcome to HiLo\n", 16, 0);
        send(p->sock, "Pick a number between 1 and 100\n", 32, 0);
        return true;
    }
    else {
        return false;
    }
}

void player_quit (Game *g, Player *p) {
    printf("Player quit\n");
    LIST_REMOVE(p, players);
    free(g->data);
}

void handle_request (Game *game, Player *p, char *req) {
    HiLoData *g = (HiLoData*)game->data;
    g->guesses++;
    long guess = strtol(req, (char **)NULL, 10);
    if (guess == g->number) {
        sprintf(&hilobuff[0],"You got it in %i tries!\n", g->guesses);
        send(p->sock, &hilobuff[0], strlen(&hilobuff[0]), 0);
        send(p->sock, "I'll choose another number\n", 27, 0);
        choose_number(g);
    }
    else if (guess > g->number) {
        send(p->sock, "Too High\n", 10, 0);
    }
    else if (guess < g->number) {
        send(p->sock, "Too low\n", 9, 0);
    }
}

void player_kicked (Game *g, Player *p, char *why) {
    printf("Player kicked for %s\n", why);
    LIST_REMOVE(p, players);
}

void player_dropped (Game *g, Player *p) {
    printf("Player dropped\n");
    LIST_REMOVE(p, players);
}
