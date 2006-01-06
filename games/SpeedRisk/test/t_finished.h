#ifndef SPEED_RISK_FINISHED_TEST_H__
#define SPEED_RISK_FINISHED_TEST_H__

#include <stdlib.h>
#include <cxxtest/TestSuite.h>
#include <SpeedRisk/protocol.h>
#include <SpeedRisk/board.h>
#include <gamed/player.h>
#include <gamed/game.h>
#include <mocks.h>

class SpeedRiskFinishedTest: public CxxTest::TestSuite {
public:
    SpeedRiskFinishedTest() {
        game.playing = 0;
        game_init (&game);
        SpeedRiskData *srd = (SpeedRiskData*)game.data;
        srd->state = SR_WAITING_FOR_PLAYERS;
        player_join (&game, &player);
        srd->state = SR_DONE;
        res = (SR_Command*)&mock_plr_buff[0];
    }

    ~SpeedRiskFinishedTest() {
        free(game.data);
    }

	void setUp()    {}
	void tearDown() {}

    void simple_command_test(int command, int exp) {
        reset_mocks();
        cmd.command = command;
        handle_request(&game, &player, (char*)&cmd);
        TS_ASSERT_EQUALS(res->command, exp);
    }

	void test_finished_commands() {
        simple_command_test(SR_CMD_NOP, SR_CMD_NOP);
        simple_command_test(SR_CMD_READY, SR_CMD_ERROR);
        simple_command_test(SR_CMD_NOTREADY, SR_CMD_ERROR);
        simple_command_test(SR_CMD_BEGIN, SR_CMD_ERROR);
        simple_command_test(SR_CMD_MOVE, SR_CMD_ERROR);
        simple_command_test(SR_CMD_ATTACK, SR_CMD_ERROR);
        simple_command_test(SR_CMD_PLACE, SR_CMD_ERROR);
        simple_command_test(SR_CMD_GAME_STATUS, SR_CMD_GAME_STATUS);
        simple_command_test(SR_CMD_COUNTRY_STATUS, SR_CMD_COUNTRY_STATUS);
    }

    Game game;
    Player player;
    SR_Command cmd;
    SR_Command *res;
};

#endif
