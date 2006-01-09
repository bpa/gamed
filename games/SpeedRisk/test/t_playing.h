#ifndef SPEED_RISK_PLAYING_TEST_H__
#define SPEED_RISK_PLAYING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <SpeedRisk/board.h>

class SpeedRiskPlayingTest: public CxxTest::TestSuite {
public:

    SpeedRiskPlayingTest() {
        srd = (SpeedRiskData*)game.data;
    }

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        srd = (SpeedRiskData*)game.data;
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];

        srd->state = SR_WAITING_FOR_PLAYERS;
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        srd->state = SR_RUNNING;

        for (int i=0; i<3; i++) {
            srd->status.countries[i].owner = i;
            srd->status.countries[i].armies = 1;
            srd->players[i].countries_held = 14;
        }
    }

	void tearDown() {
        free(game.data);
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

	void test_move() {
    }

    Game game;
    Player p1;
    Player p2;
    Player p3;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
};

#endif
