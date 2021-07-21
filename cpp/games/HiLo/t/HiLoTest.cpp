#include <test/server.h>
#include <HiLo/HiLo.h>
#include <iostream>
#include "gtest/gtest.h"

class HiLoTest: public ::testing::Test {
public:
	virtual void SetUp()    {
		game = create_instance(&HiLo);
        data = (HiLoData*)game->data;
    };

	virtual void TearDown() {
		destroy_instance(game);
    };

	GameInstance *game;
    Player p, p2;
    HiLoData *data;
};

#define guess(before, after, num, expected) { \
	cmd.subcmd = num; \
    EXPECT_EQ(before,data->guesses); \
	player_event(game, &p, (char *)&cmd, 4); \
    EXPECT_EQ((unsigned int)expected,res->command); \
    EXPECT_EQ((unsigned int)before+1,res->subcmd); \
    EXPECT_EQ(after,data->guesses); \
    reset_mocks(); \
}

TEST_F(HiLoTest, handle_request) {
	GamedCommand *res = (GamedCommand *)&mock_plr_buff[0];
	GamedCommand cmd = { HILO_GUESS, 0, 0 };
    data->number = 12;
	guess(0, 1, 0, HILO_TOO_LOW);
	guess(1, 2, 11, HILO_TOO_LOW);
	guess(2, 3, 150, HILO_TOO_HIGH);
	guess(3, 4, 13, HILO_TOO_HIGH);
	set_random(53);
	guess(4, 0, 12, HILO_CORRECT);
    EXPECT_EQ(0,data->guesses);
    EXPECT_EQ(53,data->number);
}

TEST_F(HiLoTest, init) {
	destroy_instance(game);
    set_random(100);
    game = create_instance(&HiLo);
    data = (HiLoData*)game->data;
    EXPECT_EQ(1,data->number);
    EXPECT_EQ(0,data->guesses);
    EXPECT_EQ(0,game->playing);
    destroy_instance(game);
    set_random(99);
    game = create_instance(&HiLo);
    data = (HiLoData*)game->data;
    EXPECT_EQ(100,data->number);
    EXPECT_EQ(0,data->guesses);
    EXPECT_EQ(0,game->playing);
}

TEST_F(HiLoTest, join_quit) {
	game->accepting_players = 1;
	player_join(game, &p);
	EXPECT_EQ(0, game->accepting_players);
    player_quit(game, &p);
    EXPECT_EQ(true,game_quit);
    create_instance(&HiLo);
}

