#ifndef SPEED_RISK_JOINING_TEST_H__
#define SPEED_RISK_JOINING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <SpeedRisk/board.h>
#include <test/server.h>
#include <iostream>
using namespace std;

class SpeedRiskJoiningTest: public CxxTest::TestSuite {
public:

	SpeedRiskJoiningTest() {
		init_server(&s);
		game.game = &SpeedRisk;
	}

	void setUp() {
        SpeedRisk.create(&game, &s);
        srd = (SpeedRiskData*)game.data;
        reset_mocks();
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
    }

	void tearDown() {
		s.game_over(&game);
    }

    void simple_command_all_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        game.state->player_event(&game, &s, player, (char*)&cmd, 4);
        TS_ASSERT_EQUALS(exp, all_res->command);
    }

    void simple_command_plr_test(Player *player, int command, int exp) {
        reset_mocks();
        cmd.command = command;
        game.state->player_event(&game, &s, player, (char*)&cmd, 4);
        TS_ASSERT_EQUALS(exp, plr_res->command);
    }

	void test_multi_join() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_JOIN, all_res->command);
        TS_ASSERT_EQUALS((unsigned int)0, all_res->from);
        TS_ASSERT_EQUALS(0, p1.in_game_id);
        TS_ASSERT_EQUALS(1, game.playing);
        reset_mocks();
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_JOIN, all_res->command);
        TS_ASSERT_EQUALS((unsigned int)1, all_res->from);
        TS_ASSERT_EQUALS(1, p2.in_game_id);
        TS_ASSERT_EQUALS(2, game.playing);
        reset_mocks();
        TS_ASSERT(!SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT_EQUALS(0, all_res->command);
        TS_ASSERT_EQUALS(0, p1.in_game_id);
        TS_ASSERT_EQUALS(2, game.playing);
        reset_mocks();
        TS_ASSERT(!SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT_EQUALS(0, all_res->command);
        TS_ASSERT_EQUALS(1, p2.in_game_id);
        TS_ASSERT_EQUALS(2, game.playing);
    }

	void test_join() {
        srd->players[0].ready = true;
        srd->players[1].ready = true;
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_JOIN, all_res->command);
        TS_ASSERT_EQUALS(0, p1.in_game_id);
        TS_ASSERT_EQUALS(1, game.playing);
        TS_ASSERT_EQUALS(false, srd->players[0].ready);
        simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_JOIN, all_res->command);
        TS_ASSERT_EQUALS(1, p2.in_game_id);
        TS_ASSERT_EQUALS(2, game.playing);
        TS_ASSERT_EQUALS(false, srd->players[1].ready);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        TS_ASSERT_EQUALS(&SR_PLACING, game.state);
        TS_ASSERT(!SpeedRisk.player_join(&game, &s, &p3));
        TS_ASSERT_EQUALS(2, game.playing);
    }

	void test_not_ready() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT_EQUALS(1, game.playing);
        simple_command_plr_test(&p1, SR_CMD_READY, SR_CMD_ERROR);
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        simple_command_all_test(&p1, SR_CMD_NOTREADY, SR_CMD_NOTREADY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);
        simple_command_all_test(&p2, SR_CMD_NOTREADY, SR_CMD_NOTREADY);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        TS_ASSERT_EQUALS(&SR_PLACING, game.state);
    }

    void test_drop_rejoin_out_of_order() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p3));
        SpeedRisk.player_quit(&game, &s, &p2);
        TS_ASSERT_EQUALS(0, p1.in_game_id);
        TS_ASSERT_EQUALS(2, p3.in_game_id);
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT_EQUALS(1, p2.in_game_id);
    }

    void test_can_start_with_dropped_player() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p3));
        SpeedRisk.player_quit(&game, &s, &p1);

        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);

        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        TS_ASSERT_EQUALS(&SR_PLACING, game.state);
    }

    void test_drop_unready_player_starts_game() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p3));
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);

		reset_mocks();
        SpeedRisk.player_quit(&game, &s, &p1);
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_QUIT, all_res->command);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        TS_ASSERT_EQUALS(&SR_PLACING, game.state);
    }

    void test_drop_unready_player_still_requires_two() {
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT(SpeedRisk.player_join(&game, &s, &p2));
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);

		reset_mocks();
        SpeedRisk.player_quit(&game, &s, &p1);
        TS_ASSERT_EQUALS(SR_CMD_PLAYER_QUIT, all_res->command);
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);
    }

    void test_max_players() {
        int i;
        Player players[SR_MAX_PLAYERS];
        for (i=0; i<SR_MAX_PLAYERS; i++) {
            TS_ASSERT(SpeedRisk.player_join(&game, &s, &players[i]));
            TS_ASSERT_EQUALS(i+1, game.playing);
        }
        TS_ASSERT_EQUALS(&SR_WAITING_FOR_PLAYERS, game.state);
        reset_mocks();
        TS_ASSERT_EQUALS(false, SpeedRisk.player_join(&game, &s, &p1));
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        for (i=0; i<SR_MAX_PLAYERS; i++) {
            simple_command_all_test(&players[i], SR_CMD_READY, SR_CMD_READY);
        }
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        TS_ASSERT_EQUALS(&SR_PLACING, game.state);
        for (i=0; i<SR_MAX_PLAYERS; i++) {
            LIST_REMOVE(&players[i], player);
		}
    }

    GameInstance game;
	Server s;
    Player p1;
    Player p2;
    Player p3;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
};

#endif
