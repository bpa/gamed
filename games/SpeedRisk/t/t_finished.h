#ifndef SPEED_RISK_FINISHED_TEST_H__
#define SPEED_RISK_FINISHED_TEST_H__

#include <stdlib.h>
#include <cxxtest/TestSuite.h>
#include <SpeedRisk/protocol.h>
#include <SpeedRisk/board.h>
#include <gamed/game.h>
#include <test/server.h>
#include "defs.h"

extern Game SpeedRisk;

class SpeedRiskFinishedTest: public CxxTest::TestSuite {
public:
	void setUp() {
		game = create_instance(&SpeedRisk);
        player_join(game, &player);
        game->state = &SR_DONE;
        res = (SR_Command*)&mock_plr_buff[0];
	}
	void tearDown() {
        destroy_instance(game);
	}

    void simple_command_test(int command, SR_COMMAND exp) {
        reset_mocks();
        cmd.command = command;
        player_event(game, &player, (char*)&cmd, 4);
        TS_ASSERT_EQUALS(exp, res->command);
    }

	void test_finished_commands() {
        simple_command_test(SR_CMD_READY, SR_CMD_ERROR);
        simple_command_test(SR_CMD_NOTREADY, SR_CMD_ERROR);
        simple_command_test(SR_CMD_BEGIN, SR_CMD_ERROR);
        simple_command_test(SR_CMD_MOVE, SR_CMD_ERROR);
        simple_command_test(SR_CMD_ATTACK, SR_CMD_ERROR);
        simple_command_test(SR_CMD_PLACE, SR_CMD_ERROR);
        simple_command_test(SR_CMD_GAME_STATUS, SR_CMD_GAME_STATUS);
        simple_command_test(SR_CMD_COUNTRY_STATUS, SR_CMD_COUNTRY_STATUS);
    }

    GameInstance *game;
    Player player;
    SR_Command cmd;
    SR_Command *res;
};

#endif
