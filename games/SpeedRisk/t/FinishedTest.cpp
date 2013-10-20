#include <stdlib.h>
#include <SpeedRisk/protocol.h>
#include <SpeedRisk/board.h>
#include <gamed/game.h>
#include <gtest/gtest.h>
#include <test/server.h>
#include "defs.h"

extern Game SpeedRisk;
extern void reset_mocks();

class SpeedRiskFinishedTest: public ::testing::Test {
public:
	void SetUp() {
		game = create_instance(&SpeedRisk);
        player_join(game, &player);
        game->state = &SR_DONE;
        res = (SR_Command*)&mock_plr_buff[0];
		cmd.from = 0;
	}
	void TearDown() {
        destroy_instance(game);
	}

    GameInstance *game;
    Player player;
    SR_Command cmd;
    SR_Command *res;

    void simple_command_test(int command, SR_COMMAND exp) {
        reset_mocks();
        cmd.command = command;
        player_event(game, &player, (char*)&cmd, 4);
        ASSERT_EQ(exp, res->command);
    }
};

TEST_F(SpeedRiskFinishedTest, commands) {
    simple_command_test(SR_CMD_READY, SR_CMD_ERROR);
    simple_command_test(SR_CMD_NOTREADY, SR_CMD_ERROR);
    simple_command_test(SR_CMD_BEGIN, SR_CMD_ERROR);
    simple_command_test(SR_CMD_MOVE, SR_CMD_ERROR);
    simple_command_test(SR_CMD_ATTACK, SR_CMD_ERROR);
    simple_command_test(SR_CMD_PLACE, SR_CMD_ERROR);
    simple_command_test(SR_CMD_GAME_STATUS, SR_CMD_GAME_STATUS);
    simple_command_test(SR_CMD_COUNTRY_STATUS, SR_CMD_COUNTRY_STATUS);
}
