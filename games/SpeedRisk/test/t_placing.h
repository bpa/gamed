#ifndef SPEED_RISK_PLACING_TEST_H__
#define SPEED_RISK_PLACING_TEST_H__

#include <stdlib.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <SpeedRisk/board.h>

class SpeedRiskPlacingTest: public CxxTest::TestSuite {
public:

    SpeedRiskPlacingTest() {
        srd = (SpeedRiskData*)game.data;
    }

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        srd = (SpeedRiskData*)game.data;
        srd->state = SR_WAITING_FOR_PLAYERS;
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
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

	void test_start_placing() {
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        TS_ASSERT(player_join(&game, &p3));
        TS_ASSERT(player_join(&game, &p4));
        TS_ASSERT_EQUALS(4, game.playing);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p4, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_PLACING, srd->state);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);

        SR_Game_Status *status = (SR_Game_Status*)mock_all_buff[2];
        TS_ASSERT_EQUALS(SR_CMD_GAME_STATUS, status->command.command);
        TS_ASSERT_EQUALS((1 + SR_NUM_COUNRIES) * 4, mock_all_len[2]);

        TS_ASSERT_EQUALS(30, ((SR_Command*)&mock_plr_buff[0])->armies);
        TS_ASSERT_EQUALS(30, ((SR_Command*)&mock_plr_buff[1])->armies);
        TS_ASSERT_EQUALS(30, ((SR_Command*)&mock_plr_buff[2])->armies);
        TS_ASSERT_EQUALS(30, ((SR_Command*)&mock_plr_buff[3])->armies);

        int armies[] = {0,0,0,0};

        int i;
        for (i=0; i<SR_NUM_COUNRIES; i++) {
            TS_ASSERT(status->countries[i].owner <  4);
            TS_ASSERT_EQUALS(1, status->countries[i].armies);
            armies[status->countries[i].owner]++;
        }

        int left_to_place = 0;
        for (i=0; i<4; i++) {
            TS_ASSERT(armies[i] == 10 || armies[i] == 11);
            TS_ASSERT(srd->players[i].countries_held >= 10);
            TS_ASSERT(srd->players[i].countries_held <= 11);
            TS_ASSERT(srd->players[i].armies < 12);
            TS_ASSERT_EQUALS(false, srd->players[i].ready);
            left_to_place += srd->players[i].armies;
        }
        TS_ASSERT_EQUALS(2 + game.playing * SR_STARTING_ARMIES, left_to_place);
    }

    void place(Player *p, int c, int8_t armies) {
        reset_mocks();
        cmd.command = SR_CMD_PLACE;
        cmd.to = c;
        cmd.armies = armies;
        handle_request(&game, p, (char*)&cmd, 4);
    }

    void test_place() {
        SR_Country_Status *status = (SR_Country_Status*)&mock_all_buff[0];
        error = (SR_Error*)&mock_plr_buff[0];
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_PLACING, srd->state);

        srd->status.countries[0].owner  = 0;
        srd->status.countries[1].owner  = 1;
        
        place(&p1, 0,  0);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(1, status->country.armies);
        TS_ASSERT_EQUALS(10, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);
        place(&p1, 0,  1);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(2, status->country.armies);
        TS_ASSERT_EQUALS(9, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);
        place(&p1, 0,  5);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(7, status->country.armies);
        TS_ASSERT_EQUALS(4, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);
        place(&p1, 0, -1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, error->error);
        place(&p1, 0, 20);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, error->error);
        place(&p1, 1,  1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, error->error);
    }

    void test_ready() {
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_PLACING, srd->state);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_RUNNING, srd->state);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_BEGIN, all_res->command);
    }

    Game game;
    Player p1;
    Player p2;
    Player p3;
    Player p4;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
    SR_Error   *error;
};

#endif
