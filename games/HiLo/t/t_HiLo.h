#ifndef HILO_TEST_H__
#define HILO_TEST_H__

#include <cxxtest/TestSuite.h>
#include <test/server.h>
#include <HiLo/HiLo.h>
#include <iostream>

using namespace std;
class HiLoTest: public CxxTest::TestSuite {
public:
	HiLoTest() {
        init_server(&s);
		game.game = &HiLo;
	}

	void setUp()    {
		HiLo.create(&game, &s);
        data = (HiLoData*)game.data;
    };

	void tearDown() {
        free(game.data);
    };

	#define guess(before, after, num, expected) { \
		cmd.subcmd = num; \
        TS_ASSERT_EQUALS(before,data->guesses); \
		game.state->player_event(&game, &s, &p, (char *)&cmd, 4); \
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
        free(game.data);
        set_random(100);
        HiLo.create(&game, &s);
        data = (HiLoData*)game.data;
        TS_ASSERT_EQUALS(1,data->number);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(0,game.playing);
        free(game.data);
        set_random(99);
        HiLo.create(&game, &s);
        data = (HiLoData*)game.data;
        TS_ASSERT_EQUALS(100,data->number);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(0,game.playing);
    }

    void test_join_quit() {
        bool joined = HiLo.player_join(&game, &s, &p);
        TS_ASSERT(joined);
        TS_ASSERT_EQUALS(1,game.playing);
        joined = HiLo.player_join (&game, &s, &p2);
        TS_ASSERT_EQUALS(0, joined);
        TS_ASSERT_EQUALS(1,game.playing);
        HiLo.player_quit (&game, &s, &p);
        TS_ASSERT_EQUALS(true,game_quit);
        HiLo.create(&game, &s);
    }

    Server s;
    GameInstance game;
    Player p, p2;
    HiLoData *data;
};

#endif
