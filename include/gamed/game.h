/*****************************************************************************
 * #defines and anything helpful for a game
 *****************************************************************************/
#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <gamed/player.h>

typedef struct {
    PlayerList players;
    int playing;
    char * data;
} Game;

void game_init     (Game *g);
bool player_join   (Game *g, Player *p);
void player_quit   (Game *g, Player *p);
void handle_request(Game *g, Player *p, char *);
//void player_kicked (Game *g, Player *p, char *why);
//void player_dropped(Game *g, Player *p);

void tell_player (Player *p, const char *);
//void tellf_player(Player *p, const char *, ...);
void tell_all (Game *g, const char *);
//void tellf_all(Game *g, const char *, ...);

/*
namespace Gamed {

class Game {
public:
    Game() {
        LIST_INIT(&players);
        min_players = 1;
        max_players = 1;
        num_players = 0;
    }
    virtual bool player_join (Player *p) {
        if (num_players == max_players) return false;
        LIST_INSERT_HEAD(&players, p, players);
        num_players++;
        return true;
    }
    virtual void player_quit (Player *p) {
        LIST_REMOVE(p, players);
        num_players--;
    }
    virtual void handle_request(Player *p, Request *) = 0;
    virtual void player_kicked (Player *p, char *why) { player_quit(p); }
    virtual void player_dropped(Player *p           ) { player_quit(p); }

protected:
    int min_players;
    int max_players;
    int num_players;
    PlayerList players;
};

}
#endif
*/
#endif
