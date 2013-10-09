#include <stdlib.h>

#include <gtest/gtest.h>
#include <gamed/game.h>
#include <SpeedRisk/test.h>
#include <test/server.h>
#include <iostream>

#define SR_NUM_COUNRIES 42

using namespace std;
class SpeedRiskPlacingTest: public ::testing::Test {
public:

	void SetUp()    {
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

    void place(Player *p, int c, int8_t armies) {
        reset_mocks();
        cmd.command = SR_CMD_PLACE;
        cmd.to = c;
        cmd.armies = armies;
        player_event(game, p, (char*)&cmd, 4);
    }

    GameInstance *game;
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

TEST_F(SpeedRiskPlacingTest, two_players) {
    int i;
    int countries_held[] = {0,0,0};
    player_join(game, &p1);
    player_join(game, &p2);
    ASSERT_EQ(2, game->playing);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
    SR_Game_Status *status = (SR_Game_Status*)mock_all_buff[2];
    ASSERT_EQ(SR_CMD_GAME_STATUS, status->command.command);
    ASSERT_EQ((1 + SR_NUM_COUNRIES) * 4, mock_all_len[2]);
    for (i=0; i<SR_NUM_COUNRIES; i++) {
        ASSERT_TRUE(status->countries[i].owner <  4);
        ASSERT_EQ((unsigned int)1, status->countries[i].armies);
        countries_held[status->countries[i].owner]++;
    }
    for (i=0; i<3; i++) {
        ASSERT_EQ(14, countries_held[i]);
        ASSERT_EQ((unsigned int)14, srd->players[i].countries_held);
    }
    ASSERT_EQ((unsigned int)26, srd->players[0].armies);
    ASSERT_EQ((unsigned int)26, srd->players[1].armies);
}

TEST_F(SpeedRiskPlacingTest, player_armies) {
    int i, j, count;
    unsigned int expected_held[][2]  = {{42,42}, {42,42}, {21,21}, {14,14},
                                 		{10,11}, { 8, 9}, { 7, 7} };
    int expected_total[] = { 50, 42, 40, 35, 30, 25, 20 };
    int countries_held[] = {0,0,0,0,0,0};
    ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);
    Player people[6];
    for (i=3; i<=6; i++) {
        ASSERT_EQ(&SR_WAITING_FOR_PLAYERS, game->state);
        all_res = (SR_Command*)&mock_all_buff[0];
        for (j=0; j<i; j++) {
    		player_join(game, &people[j]);
            countries_held[j] = 0;
        }
        ASSERT_EQ(i, game->playing);
		if (i < 6) {
			for (j=0; j<i; j++) {
				simple_command_all_test(&people[j], SR_CMD_READY, SR_CMD_READY);
			}
		}
        ASSERT_EQ(&SR_PLACING, game->state);
        all_res = (SR_Command*)&mock_all_buff[1];
        ASSERT_EQ(SR_CMD_START_PLACING, all_res->command);
        SR_Game_Status *status = (SR_Game_Status*)mock_all_buff[2];
        ASSERT_EQ(SR_CMD_GAME_STATUS, status->command.command);
        ASSERT_EQ((1 + SR_NUM_COUNRIES) * 4, mock_all_len[2]);
        for (j=0; j<SR_NUM_COUNRIES; j++) {
            ASSERT_TRUE(status->countries[i].owner < i);
            ASSERT_EQ((unsigned int)1, status->countries[i].armies);
            countries_held[status->countries[j].owner]++;
        }
        for (j=0; j<i; j++) {
		  ASSERT_TRUE(countries_held[j] >= 7);
          count = countries_held[j] + srd->players[j].armies;
          ASSERT_EQ(0, srd->players[j].ready);
          if (expected_total[i] != count) {
            ASSERT_EQ(expected_total[i]+1, count);
          }
          if (expected_held[i][0] != srd->players[j].countries_held) {
            ASSERT_EQ(expected_held[i][1], srd->players[j].countries_held);
          }
        }
        TearDown();
        SetUp();
    }
}

TEST_F(SpeedRiskPlacingTest, place) {
    SR_Country_Status *status = (SR_Country_Status*)&mock_all_buff[0];
    error = (SR_Error*)&mock_plr_buff[0];
    player_join(game, &p1);
    player_join(game, &p2);
    player_join(game, &p3);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);

    srd->status.countries[0].owner  = 0;
    srd->status.countries[1].owner  = 1;
    int p1_reserve = 21;
    int p2_reserve = 21;
    unsigned int c1_armies = 1;
    unsigned int c2_armies = 1;
    
    place(&p1, 0,  0);
    ASSERT_EQ(SR_CMD_COUNTRY_STATUS, all_res->command);
    ASSERT_EQ(c1_armies,  status->country.armies);
    ASSERT_EQ((unsigned int)p1_reserve, srd->players[0].armies);
    ASSERT_EQ(8, mock_all_len[0]);

    place(&p1, 0,  1); c1_armies++; p1_reserve--;
    ASSERT_EQ(SR_CMD_COUNTRY_STATUS, all_res->command);
    ASSERT_EQ(c1_armies,  status->country.armies);
    ASSERT_EQ((unsigned int)p1_reserve, srd->players[0].armies);
    ASSERT_EQ(8, mock_all_len[0]);

    place(&p1, 0,  5); c1_armies+=5; p1_reserve-=5;
    ASSERT_EQ(SR_CMD_COUNTRY_STATUS, all_res->command);
    ASSERT_EQ(c1_armies,  status->country.armies);
    ASSERT_EQ((unsigned int)p1_reserve, srd->players[0].armies);
    ASSERT_EQ(8, mock_all_len[0]);

    place(&p1, 0, -1);
    ASSERT_EQ(SR_CMD_ERROR, plr_res->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, error->error);

    place(&p1, 0, 20);
    ASSERT_EQ(SR_CMD_ERROR, plr_res->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, error->error);

    place(&p1, 1,  1); c2_armies++; p2_reserve--;
    ASSERT_EQ(SR_CMD_ERROR, plr_res->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, error->error);
}

TEST_F(SpeedRiskPlacingTest, ready) {
    player_join(game, &p1);
    player_join(game, &p2);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_RUNNING, game->state);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_BEGIN, all_res->command);
}

TEST_F(SpeedRiskPlacingTest, can_start_with_dropped_player) {
    player_join(game, &p1);
    player_join(game, &p2);
    player_join(game, &p3);

    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);
    ASSERT_EQ(0, game->accepting_players);

    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    ASSERT_EQ(&SR_PLACING, game->state);

    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);

    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_BEGIN, all_res->command);
    ASSERT_EQ(&SR_RUNNING, game->state);
}

TEST_F(SpeedRiskPlacingTest, drop_unready_player_starts_game) {
    player_join(game, &p1);
    player_join(game, &p2);
    player_join(game, &p3);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);

    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);

	reset_mocks();
    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_BEGIN, all_res->command);
    ASSERT_EQ(&SR_RUNNING, game->state);
}

TEST_F(SpeedRiskPlacingTest, last_player_wins_by_default) {
    player_join(game, &p1);
    player_join(game, &p2);
    simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
    simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
    ASSERT_EQ(&SR_PLACING, game->state);

	reset_mocks();
    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_VICTORY, all_res->command);
    ASSERT_EQ(&SR_DONE, game->state);
}
