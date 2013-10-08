#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <test/server.h>
#include <SpeedRisk/test.h>
#include <iostream>
using namespace std;

#define SR_MAX_PLAYERS 6

class SpeedRiskJoiningTest: public ::testing::Test {
public:

	void SetUp() {
        game = create_instance(&SpeedRisk);
        srd = (SpeedRiskData*)game->data;
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
    }

	void TearDown() {
		destroy_instance(game);
    }

    void simple_command_all_test(Player *player, int command, int exp) {
        cmd.command = command;
        player_event(game, player, (char*)&cmd, 4);
        ASSERT_EQ(exp, all_res->command);
    }

    void simple_command_plr_test(Player *player, int command, int exp) {
        cmd.command = command;
        player_event(game, player, (char*)&cmd, 4);
        ASSERT_EQ(exp, plr_res->command);
    }

    GameInstance *game;
    Player p1;
    Player p2;
    Player p3;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
};

TEST_F(SpeedRiskJoiningTest, multi_join) {
    player_join(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_JOIN, all_res->command);
    ASSERT_EQ((unsigned int)0, all_res->from);
    ASSERT_EQ(0, p1.in_game_id);
    ASSERT_EQ(1, game->playing);

    player_join(game, &p2);
    ASSERT_EQ(SR_CMD_PLAYER_JOIN, all_res->command);
    ASSERT_EQ((unsigned int)1, all_res->from);
    ASSERT_EQ(1, p2.in_game_id);
    ASSERT_EQ(2, game->playing);
}

TEST_F(SpeedRiskJoiningTest, join) {
    srd->players[0].ready = 1;
    srd->players[1].ready = 1;
    player_join(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_JOIN, all_res->command);
    ASSERT_EQ(0, p1.in_game_id);
    ASSERT_EQ(1, game->playing);
    ASSERT_EQ(0, srd->players[0].ready);
    ASSERT_EQ(1, game->accepting_players);
    simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
    player_join(game, &p2);
    ASSERT_EQ(SR_CMD_PLAYER_JOIN, all_res->command);
    ASSERT_EQ(1, p2.in_game_id);
    ASSERT_EQ(2, game->playing);
    ASSERT_EQ(0, srd->players[1].ready);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
	ASSERT_EQ(0, game->accepting_players);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);
}

TEST_F(SpeedRiskJoiningTest, not_ready) {
    player_join(game, &p1);
    ASSERT_EQ(1, game->playing);
    simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
    player_join(game, &p2);
    simple_command_plr_test(&p1, SR_CMD_NOTREADY, SR_CMD_ERROR);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_plr_test(&p2, SR_CMD_READY, SR_CMD_ERROR);
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);
    simple_command_all_test(&p2, SR_CMD_NOTREADY, SR_CMD_NOTREADY);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);
	ASSERT_EQ(0, game->accepting_players);
}

TEST_F(SpeedRiskJoiningTest, drop_rejoin_out_of_order) {
    player_join(game, &p1);
	ASSERT_EQ(1, game->playing);
    player_join(game, &p2);
	ASSERT_EQ(2, game->playing);
    player_join(game, &p3);
	ASSERT_EQ(3, game->playing);
    player_quit(game, &p2);
	ASSERT_EQ(2, game->playing);
    ASSERT_EQ(0, p1.in_game_id);
    ASSERT_EQ(2, p3.in_game_id);
    player_join(game, &p2);
	ASSERT_EQ(3, game->playing);
    ASSERT_EQ(1, p2.in_game_id);
}

TEST_F(SpeedRiskJoiningTest, can_start_with_dropped_player) {
    player_join(game, &p1);
    player_join(game, &p2);
    player_join(game, &p3);
    player_quit(game, &p1);

    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);

    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);
	ASSERT_EQ(0, game->accepting_players);
}

TEST_F(SpeedRiskJoiningTest, drop_unready_player_starts_game) {
    player_join(game, &p1);
    player_join(game, &p2);
    player_join(game, &p3);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);

    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);
	ASSERT_EQ(0, game->accepting_players);
}

TEST_F(SpeedRiskJoiningTest, drop_unready_player_still_requires_two) {
    player_join(game, &p1);
    player_join(game, &p2);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);

    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);
}

TEST_F(SpeedRiskJoiningTest, max_players) {
    int i;
    Player players[SR_MAX_PLAYERS];
    for (i=0; i<SR_MAX_PLAYERS; i++) {
    	player_join(game, &players[i]);
        ASSERT_EQ(i+1, game->playing);
    }
    ASSERT_EQ(0, game->accepting_players);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);
}

TEST_F(SpeedRiskJoiningTest, quit_when_all_leave) {
    player_join(game, &p1);
    ASSERT_EQ(1, game->playing);
    player_quit(game, &p1);
    ASSERT_EQ(0, all_res->command);
    ASSERT_EQ(1, game_quit);
    game = create_instance(&SpeedRisk);
    srd = (SpeedRiskData*)game->data;
}

