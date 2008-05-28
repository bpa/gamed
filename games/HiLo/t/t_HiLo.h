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
		HiLo.initialize(&game, &s);
        data = (HiLoData*)game.data;
    };

	void tearDown() {
        free(game.data);
    };

    void test_handle_request() {
        data->number = 12;
        TS_ASSERT_EQUALS(0,data->guesses);
        game.state->player_event(&game, &s, &p, (char*)"0", 2);
        TS_ASSERT_EQUALS(0,strncmp((char*)&mock_plr_buff[0], "Too low", 7));
        TS_ASSERT_EQUALS(1,data->guesses);
        reset_mocks();
        game.state->player_event(&game, &s, &p, (char*)"11", 3);
        TS_ASSERT_EQUALS(0,strncmp((char*)&mock_plr_buff[0], "Too low", 7));
        TS_ASSERT_EQUALS(2,data->guesses);
        reset_mocks();
        game.state->player_event(&game, &s, &p, (char*)"150", 4);
        TS_ASSERT_EQUALS(0,strncmp((char*)&mock_plr_buff[0], "Too high", 8));
        TS_ASSERT_EQUALS(3,data->guesses);
        reset_mocks();
        game.state->player_event(&game, &s, &p, (char*)"13", 3);
        TS_ASSERT_EQUALS(0,strncmp((char*)&mock_plr_buff[0], "Too high", 8));
        TS_ASSERT_EQUALS(4,data->guesses);
        reset_mocks();
        game.state->player_event(&game, &s, &p, (char*)"12", 4);
        TS_ASSERT_EQUALS(0,strncmp((char*)&mock_plr_buff[0], "You got it in 5 tries", 21));
        TS_ASSERT_EQUALS(0,data->guesses);
    }

    void test_game_init() {
        free(game.data);
        set_random(100);
        HiLo.initialize(&game, &s);
        data = (HiLoData*)game.data;
        TS_ASSERT_EQUALS(1,data->number);
        TS_ASSERT_EQUALS(0,data->guesses);
        TS_ASSERT_EQUALS(0,game.playing);
        free(game.data);
        set_random(99);
        HiLo.initialize(&game, &s);
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
        HiLo.initialize(&game, &s);
    }

    Server s;
    GameInstance game;
    Player p, p2;
    HiLoData *data;
};

#endif
