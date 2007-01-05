#include <stdlib.h>
#include <string.h>
#include <HiLo/HiLo.h>

typedef struct {
    int number;
    int guesses;
} HiLoData;

static void game_init     (GameInstance *g);
static bool player_join   (GameInstance *g, Player *p);
static void player_quit   (GameInstance *g, Player *p);
static void handle_request(GameInstance *g, Player *p, char *, int len);
Game HiLo = { &game_init, &player_join, &player_quit, &handle_request, NULL };

void game_init (GameInstance *g) {
    HiLoData *data = (HiLoData*)malloc(sizeof(HiLoData));
    LIST_INIT(&g->players);
    g->data = (char*)data;
    data->number = g->functions->random(100);
    data->guesses = 0;
}

bool player_join (GameInstance *g, Player *p) {
    Server *s = g->functions;
    if (LIST_EMPTY(&g->players)) {
        LIST_INSERT_HEAD(&g->players, p, player);
        g->playing = 1;
        s->tellf_player(p, "Welcome to HiLo\n");
        s->tellf_player(p, "Pick a number between 1 and 100\n");
        s->tellf_player(p, ">> ");
        return true;
    }
    else {
        return false;
    }
}

void player_quit (GameInstance *g, Player *p) {
    g->playing = 0;
    LIST_REMOVE(p, player);
}

void handle_request (GameInstance *game, Player *p, char *req, int len) {
    long guess;
    Server *s = game->functions;
    HiLoData *g = (HiLoData*)game->data;
    g->guesses++;
    guess = strtol(req, (char **)NULL, 10);
    if (guess == g->number) {
        s->tellf_player(p, "You got it in %i tries!\n", g->guesses);
        s->tellf_player(p, "I'll choose another number\n");
        g->number = s->random(100);
        g->guesses = 0;
    }
    else if (guess > g->number) {
        s->tellf_player(p, "Too High\n>> ");
    }
    else {
        s->tellf_player(p, "Too low\n>> ");
    }
}
