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
        Player *first, *next;
        first = LIST_FIRST(&game.players);
        while (first != NULL) {
            next = LIST_NEXT(first, players);
            LIST_REMOVE(first, players);
            first = next;
        }
        srd->state = SR_WAITING_FOR_PLAYERS;
        game.playing = 0;
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

    void test_two_players() {
        int i;
        int countries_held[] = {0,0,0};
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        TS_ASSERT_EQUALS(2, game.playing);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_PLACING, srd->state);
        all_res = (SR_Command*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
        SR_Game_Status *status = (SR_Game_Status*)mock_all_buff[2];
        TS_ASSERT_EQUALS(SR_CMD_GAME_STATUS, status->command.command);
        TS_ASSERT_EQUALS((1 + SR_NUM_COUNRIES) * 4, mock_all_len[2]);
        for (i=0; i<SR_NUM_COUNRIES; i++) {
            TS_ASSERT(status->countries[i].owner <  4);
            TS_ASSERT_EQUALS(1, status->countries[i].armies);
            countries_held[status->countries[i].owner]++;
        }
        for (i=0; i<3; i++) {
            TS_ASSERT_EQUALS(14, countries_held[i]);
            TS_ASSERT_EQUALS(14, srd->players[i].countries_held);
        }
        TS_ASSERT_EQUALS(26, srd->players[0].armies);
        TS_ASSERT_EQUALS(26, srd->players[1].armies);
    }

    void test_player_armies() {
        int i, j, count;
        int expected_held[][2]  = {  {42,42}, {42,42},
                                              {21,21}, {14,14},
                                     {10,11}, { 8, 9}, { 7, 7} };
        int expected_total[] = { 50, 42, 40, 35, 30, 25, 20 };
        int countries_held[] = {0,0,0,0,0,0};
        Player people[6];
        for (i=3; i<=6; i++) {
            all_res = (SR_Command*)&mock_all_buff[0];
            for (j=0; j<i; j++) {
                TS_ASSERT(player_join(&game, &people[j]));
                countries_held[j] = 0;
            }
            TS_ASSERT_EQUALS(i, game.playing);
            for (j=0; j<i; j++) {
                simple_command_all_test(&people[j], SR_CMD_READY, SR_CMD_READY);
            }
            TS_ASSERT_EQUALS(SR_PLACING, srd->state);
            all_res = (SR_Command*)&mock_all_buff[1];
            TS_ASSERT_EQUALS(SR_CMD_START_PLACING, all_res->command);
            SR_Game_Status *status = (SR_Game_Status*)mock_all_buff[2];
            TS_ASSERT_EQUALS(SR_CMD_GAME_STATUS, status->command.command);
            TS_ASSERT_EQUALS((1 + SR_NUM_COUNRIES) * 4, mock_all_len[2]);
            for (j=0; j<SR_NUM_COUNRIES; j++) {
                TS_ASSERT(status->countries[i].owner <  4);
                TS_ASSERT_EQUALS(1, status->countries[i].armies);
                countries_held[status->countries[j].owner]++;
            }
            for (j=0; j<i; j++) {
              count = countries_held[j] + srd->players[j].armies;
              TS_ASSERT_EQUALS(false, srd->players[j].ready);
              if (expected_total[i] != count) {
                TS_ASSERT_EQUALS(expected_total[i]+1, count);
              }
              if (expected_held[i][0] != srd->players[j].countries_held) {
                TS_ASSERT_EQUALS(expected_held[i][1],
                        srd->players[j].countries_held);
              }
            }
            tearDown();
            setUp();
        }
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
        TS_ASSERT(player_join(&game, &p3));
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
        TS_ASSERT_EQUALS(SR_PLACING, srd->state);

        srd->status.countries[0].owner  = 0;
        srd->status.countries[1].owner  = 1;
        int p1_reserve = 21;
        int p2_reserve = 21;
        int c1_armies = 1;
        int c2_armies = 1;
        
        place(&p1, 0,  0);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(c1_armies,  status->country.armies);
        TS_ASSERT_EQUALS(p1_reserve, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);

        place(&p1, 0,  1); c1_armies++; p1_reserve--;
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(c1_armies,  status->country.armies);
        TS_ASSERT_EQUALS(p1_reserve, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);

        place(&p1, 0,  5); c1_armies+=5; p1_reserve-=5;
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(c1_armies,  status->country.armies);
        TS_ASSERT_EQUALS(p1_reserve, srd->players[0].armies);
        TS_ASSERT_EQUALS(8, mock_all_len[0]);

        place(&p1, 0, -1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, error->error);

        place(&p1, 0, 20);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, plr_res->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, error->error);

        place(&p1, 1,  1); c2_armies++; p2_reserve--;
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
