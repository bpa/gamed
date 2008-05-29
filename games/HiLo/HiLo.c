#include <stdlib.h>
#include <string.h>
#include <gamed/game.h>
#include <HiLo/HiLo.h>

static void game_init     (GameInstance *g, Server *s);
static bool player_join   (GameInstance *g, Server *s, Player *p);
static void player_quit   (GameInstance *g, Server *s, Player *p);
static void handle_request(GameInstance *g, Server *s, Player *p, char *, int len);

Game HiLo = { "HiLo", "0.1", &game_init, &player_join, &player_quit, NULL };
State STANDARD = { NULL, &handle_request, NULL, NULL };

void game_init (GameInstance *g, Server *s) {
    HiLoData *data = (HiLoData*)malloc(sizeof(HiLoData));
    g->state = &STANDARD;
    LIST_INIT(&g->players);
    g->data = (char*)data;
    data->number = s->random(100) + 1;
    data->guesses = 0;
}

bool player_join (GameInstance *g, Server *s, Player *p) {
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

void player_quit (GameInstance *g, Server *s, Player *p) {
    s->game_over(g);
}

void handle_request (GameInstance *g, Server *s, Player *p, char *req, int len) {
    long guess;
    HiLoData *game = (HiLoData*)g->data;
    game->guesses++;
    guess = strtol(req, (char **)NULL, 10);
    if (guess == game->number) {
        s->tellf_player(p, "You got it in %i tries!\n", game->guesses);
        s->tell_player(p, "I'll choose another number\n",0);
        game->number = s->random(100);
        game->guesses = 0;
    }
	else if (guess == -1) {
		s->tell_player(p, "Thanks for playing!\n",0);
		s->game_over(g);
	}
    else if (guess > game->number) {
        s->tellf_player(p, "Too high\n>> ");
    }
    else {
        s->tellf_player(p, "Too low\n>> ");
    }
}
