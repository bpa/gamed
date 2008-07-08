#include <stdlib.h>
#include <string.h>
#include <gamed/game.h>
#include <gamed/command.h>
#include <HiLo/HiLo.h>

static void game_init     (GameInstance *g, Server *s);
static bool player_join   (GameInstance *g, Server *s, Player *p);
static void player_quit   (GameInstance *g, Server *s, Player *p);
static void handle_request(GameInstance *g, Server *s, Player *p, const char *, int len);

Game HiLo = { STANDARD_GAMED_GAME, "HiLo", "0.1", game_init, NULL, player_join, player_quit };
State STANDARD = { NULL, handle_request, NULL, NULL };

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
        return true;
    }
    else {
        return false;
    }
}

void player_quit (GameInstance *g, Server *s, Player *p) {
    s->game_over(g);
}

void handle_request (GameInstance *g, Server *s, Player *p, const char *req, int len) {
	GamedCommand cmd;
	GamedCommand *guess = (GamedCommand *)req;
    HiLoData *game = (HiLoData*)g->data;
    game->guesses++;
	cmd.subcmd = game->guesses;
	cmd.length = 0;
	switch(guess->command) {
		case HILO_GUESS:
    		if (guess->subcmd == game->number) {
				cmd.command = HILO_CORRECT;
				game->number = s->random(100);
				game->guesses = 0;
				s->tell_player(p, (char*)&cmd, 4);
			}
			else if (guess->subcmd > game->number) {
				cmd.command = HILO_TOO_HIGH;
				s->tell_player(p, (char*)&cmd, 4);
			}
			else {
				cmd.command = HILO_TOO_LOW;
				s->tell_player(p, (char*)&cmd, 4);
			}
			break;
		case HILO_GIVEUP:
			s->game_over(g);
			break;
		default:
			cmd.command = CMD_INVALID;
			s->tell_player(p, req, 4);
	}
}
