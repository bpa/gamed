/******************************************************************************
  #defines and anything helpful for a game
******************************************************************************/
#ifndef GAMED_GAME_H
#define GAMED_GAME_H

#include <gamed/player.h>
#include <gamed/request.h>

namespace Gamed {

class Game {
public:
    virtual bool player_join(Player *);
    virtual void player_quit(Player *);
    virtual void handle_request(Player *, Request *) = 0;

protected:
    int min_players;
    int max_players;
    PlayerList players;
};

}
#endif
