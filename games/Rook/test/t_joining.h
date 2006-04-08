#ifndef ROOK_JOINING_TEST_H__
#define ROOK_JOINING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <Rook/game.h>

class RookJoiningTest: public CxxTest::TestSuite {
public:

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        rook = (Rook*)game.data;
        rook->state = ROOK_WAITING_FOR_PLAYERS;
        plr_res = (RookCommand*)&mock_plr_buff[0];
        all_res = (RookCommand*)&mock_all_buff[0];
    }

	void tearDown() {
        free(game.data);
        game.data = NULL;
    }

    void simple_command_all_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        handle_request(&game, player, (char*)&cmd, 4);
        TS_ASSERT_EQUALS(exp, all_res->command);
    }

    void simple_command_plr_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        handle_request(&game, player, (char*)&cmd, 4);
        TS_ASSERT_EQUALS(exp, plr_res->command);
    }

    void test_fill_game() {
        int i;
        Player players[4];
        for (i=0; i<4; i++) {
            reset_mocks();
            TS_ASSERT(player_join(&game, &players[i]));
            TS_ASSERT_EQUALS(i+1, game.playing);
            TS_ASSERT_EQUALS(ROOK_CMD_PLAYER_JOIN, all_res->command);
        }
        all_res = (RookCommand*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(ROOK_CMD_BIDDING, all_res->command);
        TS_ASSERT_EQUALS(ROOK_BIDDING, rook->state);
    }

    Game game;
    Player p1;
    Player p2;
    Player p3;
    Rook *rook;
    RookCommand cmd;
    RookCommand *plr_res;
    RookCommand *all_res;
};

#endif
