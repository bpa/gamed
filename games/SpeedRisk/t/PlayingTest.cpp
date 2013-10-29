#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <gamed/game.h>
#include <SpeedRisk/test.h>
#include <test/server.h>

#define cmd_res(i) ((SR_Command*)&mock_plr_buff[i])

class SpeedRiskPlayingTest: public ::testing::Test {
public:

	void SetUp()    {
        game = create_instance(&SpeedRisk);
        srd = (SpeedRiskData*)game->data;
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
        err = (SR_Error*)&mock_plr_buff[0];
		cmd.to = cmd.from = 0;

        player_join(game, &p1);
        ASSERT_EQ(&p1, srd->players[0].player);
        player_join(game, &p2);
        ASSERT_EQ(&p2, srd->players[1].player);
        player_join(game, &p3);
        ASSERT_EQ(&p3, srd->players[2].player);
        ASSERT_EQ((Player*)NULL, srd->players[3].player);
        ASSERT_EQ((Player*)NULL, srd->players[4].player);
        ASSERT_EQ((Player*)NULL, srd->players[5].player);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
        ASSERT_EQ(&SR_PLACING, game->state);
        simple_command_all_test(&p1, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p2, SR_CMD_READY, SR_CMD_READY);
        simple_command_all_test(&p3, SR_CMD_READY, SR_CMD_READY);
        ASSERT_EQ(&SR_RUNNING, game->state);

        for (int i=0; i<3; i++) {
            srd->status.countries[i].owner = i;
            srd->status.countries[i].armies = 1;
            srd->players[i].countries_held = 14;
        }
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

    void send_cmd(Player *player, int command, int from, int to, int armies) {
        cmd.command = command;
        cmd.from = from;
        cmd.to = to;
        cmd.armies = armies;
        player_event(game, player, (char*)&cmd, 4);
    }

    void setup_country(int c, int owner, int armies) {
        srd->status.countries[c].owner = owner;
        srd->status.countries[c].armies = armies;
    }

    void clear_board() {
        //This is a completely invalid state, but useful
        for(int i=0; i<SR_NUM_COUNTRIES; i++) {
            srd->status.countries[i].owner = 0;
            srd->status.countries[i].armies = 1;
        }
        for(int i=0; i<3; i++) {
            srd->players[i].countries_held = 1;
            srd->players[i].armies = 0;
        }
        reset_mocks();
    }

    GameInstance *game;
    Player p1;
    Player p2;
    Player p3;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
    SR_Error *err;
};

TEST_F(SpeedRiskPlayingTest, move) {
    setup_country(SR_EASTERN_AUSTRALIA, 0, 2);
    setup_country(SR_WESTERN_AUSTRALIA, 1, 3);
    setup_country(SR_NEW_GUINEA,        1, 1);
    setup_country(SR_MADAGASCAR,        1, 1);

    //Normal move
    SR_Move_Result *mv = (SR_Move_Result*)all_res;
    send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ(SR_CMD_MOVE_RESULT, all_res->command);
    ASSERT_EQ(SR_WESTERN_AUSTRALIA, (int)mv->country1.country);
    ASSERT_EQ((unsigned int)1, mv->country1.owner);
    ASSERT_EQ((unsigned int)1, mv->country1.armies);
    ASSERT_EQ(SR_NEW_GUINEA,        (int)mv->country2.country);
    ASSERT_EQ((unsigned int)1, mv->country2.owner);
    ASSERT_EQ((unsigned int)3, mv->country2.armies);

    //Can't move last guy or more than you have
    send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 3);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 4);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 4);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    
    //Can't move to same space
    send_cmd(&p1, SR_CMD_MOVE, 0, 0, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_INVALID_DESTINATION, err->error);

    //Can't move to non-bordering country
    send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_MADAGASCAR, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_INVALID_DESTINATION, err->error);

    //Can't move to or from country owned by someone else
    send_cmd(&p1, SR_CMD_MOVE, SR_NEW_GUINEA, SR_EASTERN_AUSTRALIA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
    send_cmd(&p1, SR_CMD_MOVE, SR_EASTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);

    //Can't move someone elses armies
    send_cmd(&p1, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
    send_cmd(&p1, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
}

TEST_F(SpeedRiskPlayingTest, single_valid_attacks) {
    SR_Move_Result *mv = (SR_Move_Result*)all_res;
    setup_country(SR_WESTERN_AUSTRALIA, 0, 5);
    setup_country(SR_NEW_GUINEA,        1, 5);

    //Attacker wins
    set_random(5,1);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)SR_WESTERN_AUSTRALIA, mv->country1.country);
    ASSERT_EQ((unsigned int)0, mv->country1.owner);
    ASSERT_EQ((unsigned int)5, mv->country1.armies);
    ASSERT_EQ((unsigned int)SR_NEW_GUINEA,        mv->country2.country);
    ASSERT_EQ((unsigned int)1, mv->country2.owner);
    ASSERT_EQ((unsigned int)4, mv->country2.armies);

    //Tie
    set_random(5,5);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)4, mv->country1.armies);
    ASSERT_EQ((unsigned int)4, mv->country2.armies);

    //Attacker loses
    set_random(1,5);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)3, mv->country1.armies);
    ASSERT_EQ((unsigned int)4, mv->country2.armies);
}

TEST_F(SpeedRiskPlayingTest, attack_order) {
    SR_Move_Result *mv = (SR_Move_Result*)all_res;
    setup_country(SR_WESTERN_AUSTRALIA, 0, 10);
    setup_country(SR_NEW_GUINEA,        1, 10);

    //Attacker wins
    set_random(2,3,5,2,3);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)10, mv->country1.armies);
    ASSERT_EQ((unsigned int) 8, mv->country2.armies);
}

TEST_F(SpeedRiskPlayingTest, multi_valid_attacks) {
    SR_Move_Result *mv = (SR_Move_Result*)all_res;
    setup_country(SR_WESTERN_AUSTRALIA, 0, 10);
    setup_country(SR_NEW_GUINEA,        1, 10);

    //Attacker wins
    set_random(5,3,4,2);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)10, mv->country1.armies);
    ASSERT_EQ((unsigned int) 8, mv->country2.armies);

    //Attacker wins & ties
    set_random(5,5,5,4);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)9, mv->country1.armies);
    ASSERT_EQ((unsigned int)7, mv->country2.armies);

    //Attacker ties & loses
    set_random(3,5,5,4);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)7, mv->country1.armies);
    ASSERT_EQ((unsigned int)7, mv->country2.armies);

    //Attacker loses
    set_random(3,2,5,4);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ(SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)5, mv->country1.armies);
    ASSERT_EQ((unsigned int)7, mv->country2.armies);
}

TEST_F(SpeedRiskPlayingTest, bad_attacks) {
    setup_country(SR_SIAM,              0, 1);
    setup_country(SR_EASTERN_AUSTRALIA, 0, 2);
    setup_country(SR_WESTERN_AUSTRALIA, 0, 3);
    setup_country(SR_INDONESIA,         1, 1);
    setup_country(SR_NEW_GUINEA,        1, 1);
    setup_country(SR_MADAGASCAR,        1, 1);

    //Can't attack with no one
    send_cmd(&p1, SR_CMD_ATTACK, SR_SIAM, SR_INDONESIA, 0);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

    //Can't attack with last guy or more than you have
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 3);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 4);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 4);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
    
    //Can't attack yourself
    send_cmd(&p1, SR_CMD_ATTACK, 0, 0, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_INVALID_DESTINATION, err->error);

    send_cmd(&p2, SR_CMD_ATTACK, SR_INDONESIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_INVALID_DESTINATION, err->error);

    //Can't attack to non-bordering country
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_MADAGASCAR, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_INVALID_DESTINATION, err->error);

    //Can't attack unless you own from
    send_cmd(&p1, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_EASTERN_AUSTRALIA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
    send_cmd(&p2, SR_CMD_ATTACK, SR_EASTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);

    //Can't attack for someone else
    send_cmd(&p2, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
    send_cmd(&p1, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
}

TEST_F(SpeedRiskPlayingTest, place) {
    SR_Country_Status *status = (SR_Country_Status*)&mock_all_buff[0];
    
    //Don't have armies to place
    srd->players[0].armies = 0;
    send_cmd(&p1, SR_CMD_PLACE, 0, 0, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

    srd->players[0].armies = 10;
    //Try to place more than you have
    send_cmd(&p1, SR_CMD_PLACE, 0, 0, 16);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

    //Normal placement
    send_cmd(&p1, SR_CMD_PLACE, 0, 0, 1);
    ASSERT_EQ(SR_CMD_COUNTRY_STATUS, all_res->command);
    ASSERT_EQ((unsigned int)2, status->country.armies);
    ASSERT_EQ((unsigned int)9, srd->players[0].armies);

    send_cmd(&p1, SR_CMD_PLACE, 0, 0, 5);
    ASSERT_EQ(SR_CMD_COUNTRY_STATUS, all_res->command);
    ASSERT_EQ((unsigned int)7, status->country.armies);
    ASSERT_EQ((unsigned int)4, srd->players[0].armies);

    //Negative armies
    send_cmd(&p1, SR_CMD_PLACE, 0, 0, -1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

    //Can't place on other people's countries
    send_cmd(&p1, SR_CMD_PLACE, 0, 1, 1);
    ASSERT_EQ(SR_CMD_ERROR, err->command);
    ASSERT_EQ(SR_ERR_NOT_OWNER, err->error);
}

TEST_F(SpeedRiskPlayingTest, produce_armies_country) {
    clear_board();
    srd->status.countries[SR_EASTERN_US].owner = 1;
    srd->status.countries[SR_VENEZUELA].owner = 1;
    srd->status.countries[SR_NORTH_AFRICA].owner = 1;
    srd->status.countries[SR_ICELAND].owner = 1;
    srd->status.countries[SR_JAPAN].owner = 1;
    srd->status.countries[SR_NEW_GUINEA].owner = 1;

    srd->players[1].countries_held = 11;
    srd->players[2].countries_held = 12;
    produce_armies(game, get_server());

    ASSERT_EQ((unsigned int)3, cmd_res(0)->armies);
    ASSERT_EQ((unsigned int)3, cmd_res(1)->armies);
    ASSERT_EQ((unsigned int)4, cmd_res(2)->armies);

    for(int i=0; i<3; i++) {
        srd->players[i].countries_held = 30 + i;
        srd->players[i].armies = 0;
    }
    reset_mocks();
    produce_armies(game, get_server());

    ASSERT_EQ((unsigned int)10, cmd_res(0)->armies);
    ASSERT_EQ((unsigned int)10, cmd_res(1)->armies);
    ASSERT_EQ((unsigned int)10, cmd_res(2)->armies);
}

TEST_F(SpeedRiskPlayingTest, produce_armies_player_gaps) {
    clear_board();
	srd->players[1].player = NULL;
	srd->players[2].player = &p2;
	srd->players[4].player = &p3;
	for (int i=0; i<=4; i++) {
		srd->players[i].armies = 0;
		srd->players[i].countries_held = 15;
	}
    produce_armies(game, get_server());
	ASSERT_EQ(mock_plr_pos, 3);
    ASSERT_LT((unsigned int)5, srd->players[0].armies);
    ASSERT_LT((unsigned int)5, cmd_res(0)->armies);
    ASSERT_EQ((unsigned int)0, srd->players[1].armies);
    ASSERT_EQ((unsigned int)5, srd->players[2].armies);
    ASSERT_EQ((unsigned int)5, cmd_res(1)->armies);
    ASSERT_EQ((unsigned int)0, srd->players[3].armies);
    ASSERT_EQ((unsigned int)5, srd->players[4].armies);
    ASSERT_EQ((unsigned int)5, cmd_res(2)->armies);
}

TEST_F(SpeedRiskPlayingTest, produce_min_max_armies) {
    clear_board();
    srd->players[0].armies = 250;
    srd->players[2].armies = 0;
    srd->players[0].countries_held = 40;
    srd->players[2].countries_held = 0;
    produce_armies(game, get_server());
    ASSERT_EQ((unsigned int)255, srd->players[0].armies);
    ASSERT_EQ((unsigned int)255, cmd_res(0)->armies);
    ASSERT_EQ((unsigned int)0,   srd->players[2].armies);
    ASSERT_EQ((unsigned int)0,   cmd_res(2)->armies);
}

TEST_F(SpeedRiskPlayingTest, produce_armies_continent) {
    SR_Game_Status *st = &srd->status;
    clear_board();
    st->countries[SR_EASTERN_US].owner = 1;
    st->countries[SR_VENEZUELA].owner = 1;
    st->countries[SR_NEW_GUINEA].owner = 1;

    st->countries[SR_ICELAND].owner = 1;
    st->countries[SR_SOUTHERN_EUROPE].owner = 1;
    st->countries[SR_UKRAINE].owner = 1;
    st->countries[SR_SCANDINAVIA].owner = 1;
    st->countries[SR_GREAT_BRITAIN].owner = 1;
    st->countries[SR_WESTERN_EUROPE].owner = 1;
    st->countries[SR_NORTHERN_EUROPE].owner = 1;

    st->countries[SR_EGYPT].owner = 2;
    st->countries[SR_CONGO].owner = 2;
    st->countries[SR_MADAGASCAR].owner = 2;
    st->countries[SR_SOUTH_AFRICA].owner = 2;
    st->countries[SR_EAST_AFRICA].owner = 2;
    st->countries[SR_NORTH_AFRICA].owner = 2;

    produce_armies(game, get_server());
    ASSERT_EQ((unsigned int)10, cmd_res(0)->armies); //3 + Asia (7) = 10
    ASSERT_EQ((unsigned int) 8, cmd_res(1)->armies); //3 + Europe(5) = 8
    ASSERT_EQ((unsigned int) 6, cmd_res(2)->armies); //3 + Africa(3) = 6

    clear_board();
    st->countries[SR_NORTH_AFRICA].owner = 1;
    st->countries[SR_ICELAND].owner = 1;
    st->countries[SR_JAPAN].owner = 1;

    st->countries[SR_BRAZIL].owner = 1;
    st->countries[SR_VENEZUELA].owner = 1;
    st->countries[SR_ARGENTINA].owner = 1;
    st->countries[SR_PERU].owner = 1;

    st->countries[SR_NEW_GUINEA].owner = 2;
    st->countries[SR_INDONESIA].owner = 2;
    st->countries[SR_WESTERN_AUSTRALIA].owner = 2;
    st->countries[SR_EASTERN_AUSTRALIA].owner = 2;

    produce_armies(game, get_server());
    ASSERT_EQ((unsigned int)8, cmd_res(0)->armies); //3 + N. Am(5) = 8
    ASSERT_EQ((unsigned int)5, cmd_res(1)->armies); //3 + S. Am(2) = 5
    ASSERT_EQ((unsigned int)5, cmd_res(2)->armies); //3 + Austr(2) = 5
}

TEST_F(SpeedRiskPlayingTest, conquer_country) {
    SR_Move_Result *mv = (SR_Move_Result*)all_res;

    //Attacker wins
    setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
    setup_country(SR_NEW_GUINEA,        1, 1);
    set_random(5,5,5,1);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)0, mv->country1.owner);
    ASSERT_EQ((unsigned int)6, mv->country1.armies);
    ASSERT_EQ((unsigned int)0, mv->country2.owner);
    ASSERT_EQ((unsigned int)3, mv->country2.armies);
    ASSERT_EQ((unsigned int)15, srd->players[0].countries_held);
    ASSERT_EQ((unsigned int)13, srd->players[1].countries_held);

    //Attacker wins
    setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
    setup_country(SR_NEW_GUINEA,        1, 2);
    set_random(5,5,4,4);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
    ASSERT_EQ((int)SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)0, mv->country1.owner);
    ASSERT_EQ((unsigned int)7, mv->country1.armies);
    ASSERT_EQ((unsigned int)0, mv->country2.owner);
    ASSERT_EQ((unsigned int)2, mv->country2.armies);

    //Attacker wins
    setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
    setup_country(SR_NEW_GUINEA,        1, 1);
    set_random(5,5,5,1);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 8);
    ASSERT_EQ(SR_CMD_ATTACK_RESULT, mv->command.command);
    ASSERT_EQ((unsigned int)0, mv->country1.owner);
    ASSERT_EQ((unsigned int)1, mv->country1.armies);
    ASSERT_EQ((unsigned int)0, mv->country2.owner);
    ASSERT_EQ((unsigned int)8, mv->country2.armies);
}

TEST_F(SpeedRiskPlayingTest, win_game) {
    SR_Move_Result *mv = (SR_Move_Result*)all_res;
    int i;
    for (i=0; i<42; i++) {
        srd->status.countries[i].owner = 0;
        srd->status.countries[i].armies = 1;
    }
    srd->players[0].countries_held = 41;
    srd->players[1].countries_held = 1;
    srd->players[2].countries_held = 0;
    srd->players[3].countries_held = 0;

    setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
    setup_country(SR_NEW_GUINEA,        1, 1);
    set_random(5,5,4,4);
    send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 8);
    ASSERT_EQ(SR_CMD_ATTACK_RESULT, mv->command.command);
    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_DEFEAT, all_res->command);
    ASSERT_EQ((unsigned int)1, all_res->from);
    ASSERT_EQ((unsigned int)0, srd->players[1].countries_held);
    all_res = (SR_Command*)&mock_all_buff[2];
    ASSERT_EQ(SR_CMD_VICTORY, all_res->command);
    ASSERT_EQ((unsigned int)0, all_res->from);
    ASSERT_EQ((unsigned int)42, srd->players[0].countries_held);

    ASSERT_EQ(&SR_DONE, game->state);
}

TEST_F(SpeedRiskPlayingTest, last_player_wins_by_default) {
    player_quit(game, &p1);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);
    player_quit(game, &p2);
    ASSERT_EQ(SR_CMD_PLAYER_QUIT, all_res->command);

    all_res = (SR_Command*)&mock_all_buff[1];
    ASSERT_EQ(SR_CMD_VICTORY, all_res->command);
    ASSERT_EQ((unsigned int)p3.in_game_id, all_res->from);
    ASSERT_EQ(&SR_DONE, game->state);
}
