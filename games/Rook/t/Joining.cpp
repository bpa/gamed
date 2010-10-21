#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <test/server.h>
#include "test.h"

class RookJoiningTest: public ::testing::Test {
public:
	void SetUp()    {
		game = create_instance(&Rook);
        rook = (RookData*)game->data;
        plr_res = (RookCommand*)&mock_plr_buff[0];
        all_res = (RookCommand*)&mock_all_buff[0];
    }

	void tearDown() {
        destroy_instance(game);
    }

    void simple_command_all_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        player_event(game, player, (char*)&cmd, 4);
        ASSERT_EQ(exp, all_res->command);
    }

    void simple_command_plr_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        player_event(game, player, (char*)&cmd, 4);
        ASSERT_EQ(exp, plr_res->command);
    }

    /**
     * make sure game got set up correctly
     */
	void test_init_game() {
        ASSERT_EQ(&WAITING, game->state);
	}

    Player players[4];
    GameInstance *game;
    RookData *rook;
	RookCommand cmd;
    RookCommand *plr_res;
    RookCommand *all_res;
};

/**
 * 4 players join and the games starts automatically
 */
TEST_F(RookJoiningTest, start_game) {
    int i;
    Player players[4];
    for (i=0; i<4; i++) {
        reset_mocks();
        player_join(game, &players[i]);
        ASSERT_EQ(i+1, game->playing);
        ASSERT_EQ(ROOK_CMD_PLAYER_JOIN, all_res->command);
    }
    all_res = (RookCommand*)&mock_all_buff[1];
    ASSERT_EQ(ROOK_CMD_BIDDING, all_res->command);
    ASSERT_EQ(&BIDDING, game->state);
}

/**
 * Have one player start a single player game
 * The game should move into bidding
 */
TEST_F(RookJoiningTest, start_single_player) {
    //Player *p;
    //cmd = { ROOK_CMD_READY, 0, 0, 0 };
    //player_join(game, p);
    //player_event(game, p, &cmd);
}
