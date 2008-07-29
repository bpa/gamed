#ifndef HILO_TEST_H__
#define HILO_TEST_H__

#include <cxxtest/TestSuite.h>
#include <test/server.h>
#include <HiLo/HiLo.h>
#include <iostream>

using namespace std;
class HiLoTest: public CxxTest::TestSuite {
public:
	void setUp()    {
		game = create_instance(&HiLo);
        data = (HiLoData*)game->data;
    };

	void tearDown() {
		destroy_instance(game);
    };

	#define guess(before, after, num, expected) { \
		cmd.subcmd = num; \
        TS_ASSERT_EQUALS(before,data->guesses); \
		player_event(game, &p, (char *)&cmd, 4); \
        TS_ASSERT_EQUALS((unsigned int)expected,res->command); \
        TS_ASSERT_EQUALS((unsigned int)before+1,res->subcmd); \
        TS_ASSERT_EQUALS(after,data->guesses); \
        reset_mocks(); \
	}

    void test_handle_request() {
		GamedCommand *res = (GamedCommand *)&mock_plr_buff[0];
		GamedCommand cmd = { HILO_GUESS, 0, 0 };
        data->number = 12;
		guess(0, 1, 0, HILO_TOO_LOW);
		guess(1, 2, 11, HILO_TOO_LOW);
		guess(2, 3, 150, HILO_TOO_HIGH);
		guess(3, 4, 13, HILO_TOO_HIGH);
		set_random(53);
		guess(4, 0, 12, HILO_CORRECT);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(53,data->number);
    }

    void test_game_init() {
		destroy_instance(game);
        set_random(100);
        game = create_instance(&HiLo);
        data = (HiLoData*)game->data;
        TS_ASSERT_EQUALS(1,data->number);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(0,game->playing);
        destroy_instance(game);
        set_random(99);
        game = create_instance(&HiLo);
        data = (HiLoData*)game->data;
        TS_ASSERT_EQUALS(100,data->number);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(0,game->playing);
    }

    void test_join_quit() {
		game->accepting_players = 1;
		player_join(game, &p);
		TS_ASSERT_EQUALS(0, game->accepting_players);
        player_quit(game, &p);
        TS_ASSERT_EQUALS(true,game_quit);
        create_instance(&HiLo);
    }

	GameInstance *game;
    Player p, p2;
    HiLoData *data;
};

#endif
