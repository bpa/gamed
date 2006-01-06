#ifndef SPEED_RISK_JOINING_TEST_H__
#define SPEED_RISK_JOINING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <SpeedRisk/board.h>

class SpeedRiskJoiningTest: public CxxTest::TestSuite {
public:

    SpeedRiskJoiningTest() {
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
        srd = (SpeedRiskData*)game.data;
    }

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        srd = (SpeedRiskData*)game.data;
        srd->state = SR_WAITING_FOR_PLAYERS;
    }

	void tearDown() {
        free(game.data);
    }

    void simple_command_all_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        handle_request(&game, player, (char*)&cmd);
        TS_ASSERT_EQUALS(exp, plr_res->command);
    }

    void simple_command_plr_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        handle_request(&game, player, (char*)&cmd);
        TS_ASSERT_EQUALS(exp, all_res->command);
    }

	void test_join() {
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT_EQUALS(1, game.playing);
        simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
        TS_ASSERT(player_join(&game, &p2));
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        all_res++;
        TS_ASSERT_EQUALS(SR_CMD_BEGIN, all_res->command);
        TS_ASSERT_EQUALS(SR_RUNNING, srd->state);
        TS_ASSERT(!player_join(&game, &p3));
        TS_ASSERT_EQUALS(2, game.playing);
    }

	void test_not_ready() {
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT_EQUALS(1, game.playing);
        simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
        TS_ASSERT(player_join(&game, &p2));
        simple_command_all_test(&p1, SR_CMD_NOTREADY, SR_CMD_NOTREADY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_WAITING_FOR_PLAYERS, srd->state);
        simple_command_all_test(&p2, SR_CMD_NOTREADY, SR_CMD_NOTREADY);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_WAITING_FOR_PLAYERS, srd->state);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        all_res++;
        TS_ASSERT_EQUALS(SR_CMD_BEGIN, all_res->command);
        TS_ASSERT_EQUALS(SR_RUNNING, srd->state);
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
