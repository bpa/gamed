#ifndef SPEED_RISK_PLAYING_TEST_H__
#define SPEED_RISK_PLAYING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <SpeedRisk/board.h>
#include <SpeedRisk/protocol.h>

#define cmd_res(i) ((SR_Command*)&mock_plr_buff[i])

class SpeedRiskPlayingTest: public CxxTest::TestSuite {
public:

    SpeedRiskPlayingTest() {
        srd = (SpeedRiskData*)game.data;
    }

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        srd = (SpeedRiskData*)game.data;
        plr_res = (SR_Command*)&mock_plr_buff[0];
        all_res = (SR_Command*)&mock_all_buff[0];
        err = (SR_Error*)&mock_plr_buff[0];

        srd->state = SR_WAITING_FOR_PLAYERS;
        TS_ASSERT(player_join(&game, &p1));
        TS_ASSERT(player_join(&game, &p2));
        TS_ASSERT(player_join(&game, &p3));
        srd->state = SR_RUNNING;

        for (int i=0; i<3; i++) {
            srd->status.countries[i].owner = i;
            srd->status.countries[i].armies = 1;
            srd->players[i].countries_held = 14;
        }
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

    void send_cmd(Player *player, int command, int from, int to, int armies) {
        reset_mocks();
        cmd.command = command;
        cmd.from = from;
        cmd.to = to;
        cmd.armies = armies;
        handle_request(&game, player, (char*)&cmd, 4);
    }

    void setup_country(int c, int owner, int armies) {
        srd->status.countries[c].owner = owner;
        srd->status.countries[c].armies = armies;
    }

    void test_borders() {
        //positive in both directions
        TS_ASSERT(borders(SR_CENTRAL_AMERICA, SR_VENEZUELA));
        TS_ASSERT(borders(SR_VENEZUELA, SR_CENTRAL_AMERICA));

        //negative in both directions
        TS_ASSERT(!borders(SR_ICELAND, SR_MADAGASCAR));
        TS_ASSERT(!borders(SR_MADAGASCAR, SR_ICELAND));

        //can't border self
        TS_ASSERT(!borders(SR_ARGENTINA, SR_ARGENTINA));
    }

	void test_move() {
        setup_country(SR_EASTERN_AUSTRALIA, 0, 2);
        setup_country(SR_WESTERN_AUSTRALIA, 1, 3);
        setup_country(SR_NEW_GUINEA,        1, 1);
        setup_country(SR_MADAGASCAR,        1, 1);

        //Normal move
        SR_Move_Result *mv = (SR_Move_Result*)all_res;
        send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS(SR_CMD_MOVE_RESULT, all_res->command);
        TS_ASSERT_EQUALS(SR_WESTERN_AUSTRALIA, (int)mv->country1.country);
        TS_ASSERT_EQUALS(1, mv->country1.owner);
        TS_ASSERT_EQUALS(1, mv->country1.armies);
        TS_ASSERT_EQUALS(SR_NEW_GUINEA,        (int)mv->country2.country);
        TS_ASSERT_EQUALS(1, mv->country2.owner);
        TS_ASSERT_EQUALS(3, mv->country2.armies);

        //Can't move last guy or more than you have
        send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 3);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 4);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 4);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        
        //Can't move to same space
        send_cmd(&p1, SR_CMD_MOVE, 0, 0, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_INVALID_DESTINATION, err->error);

        //Can't move to non-bordering country
        send_cmd(&p2, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_MADAGASCAR, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_INVALID_DESTINATION, err->error);

        //Can't move to or from country owned by someone else
        send_cmd(&p1, SR_CMD_MOVE, SR_NEW_GUINEA, SR_EASTERN_AUSTRALIA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
        send_cmd(&p1, SR_CMD_MOVE, SR_EASTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);

        //Can't move someone elses armies
        send_cmd(&p1, SR_CMD_MOVE, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
        send_cmd(&p1, SR_CMD_MOVE, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
    }

	void test_single_valid_attacks() {
        SR_Move_Result *mv = (SR_Move_Result*)all_res;
        setup_country(SR_WESTERN_AUSTRALIA, 0, 5);
        setup_country(SR_NEW_GUINEA,        1, 5);

        //Attacker wins
        set_random(5,1);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS((int)SR_WESTERN_AUSTRALIA, mv->country1.country);
        TS_ASSERT_EQUALS(0, mv->country1.owner);
        TS_ASSERT_EQUALS(5, mv->country1.armies);
        TS_ASSERT_EQUALS((int)SR_NEW_GUINEA,        mv->country2.country);
        TS_ASSERT_EQUALS(1, mv->country2.owner);
        TS_ASSERT_EQUALS(4, mv->country2.armies);

        //Tie
        set_random(5,5);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(4, mv->country1.armies);
        TS_ASSERT_EQUALS(4, mv->country2.armies);

        //Attacker loses
        set_random(1,5);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(3, mv->country1.armies);
        TS_ASSERT_EQUALS(4, mv->country2.armies);
    }

	void test_attack_order() {
        SR_Move_Result *mv = (SR_Move_Result*)all_res;
        setup_country(SR_WESTERN_AUSTRALIA, 0, 10);
        setup_country(SR_NEW_GUINEA,        1, 10);

        //Attacker wins
        set_random(2,3,5,2,3);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(10, mv->country1.armies);
        TS_ASSERT_EQUALS( 8, mv->country2.armies);
    }

	void test_multi_valid_attacks() {
        SR_Move_Result *mv = (SR_Move_Result*)all_res;
        setup_country(SR_WESTERN_AUSTRALIA, 0, 10);
        setup_country(SR_NEW_GUINEA,        1, 10);

        //Attacker wins
        set_random(5,3,4,2);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(10, mv->country1.armies);
        TS_ASSERT_EQUALS( 8, mv->country2.armies);

        //Attacker wins & ties
        set_random(5,5,5,4);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(9, mv->country1.armies);
        TS_ASSERT_EQUALS(7, mv->country2.armies);

        //Attacker ties & loses
        set_random(3,5,5,4);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(7, mv->country1.armies);
        TS_ASSERT_EQUALS(7, mv->country2.armies);

        //Attacker loses
        set_random(3,2,5,4);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS(SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(5, mv->country1.armies);
        TS_ASSERT_EQUALS(7, mv->country2.armies);
    }

	void test_bad_attacks() {
        setup_country(SR_EASTERN_AUSTRALIA, 0, 2);
        setup_country(SR_WESTERN_AUSTRALIA, 0, 3);
        setup_country(SR_INDONESIA,         1, 1);
        setup_country(SR_NEW_GUINEA,        1, 1);
        setup_country(SR_MADAGASCAR,        1, 1);

        //Can't attack with last guy or more than you have
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 3);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p2, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 4);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 4);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);
        
        //Can't attack yourself
        send_cmd(&p1, SR_CMD_ATTACK, 0, 0, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_INVALID_DESTINATION, err->error);

        send_cmd(&p2, SR_CMD_ATTACK, SR_INDONESIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_INVALID_DESTINATION, err->error);

        //Can't attack to non-bordering country
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_MADAGASCAR, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_INVALID_DESTINATION, err->error);

        //Can't attack unless you own from
        send_cmd(&p1, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_EASTERN_AUSTRALIA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
        send_cmd(&p2, SR_CMD_ATTACK, SR_EASTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);

        //Can't attack for someone else
        send_cmd(&p2, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
        send_cmd(&p1, SR_CMD_ATTACK, SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
    }

    void test_place() {
        SR_Country_Status *status = (SR_Country_Status*)&mock_all_buff[0];
        
        //Don't have armies to place
        send_cmd(&p1, SR_CMD_PLACE, 0, 0, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

        srd->players[0].armies = 10;
        //Try to place more than you have
        send_cmd(&p1, SR_CMD_PLACE, 0, 0, 16);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

        //Normal placement
        send_cmd(&p1, SR_CMD_PLACE, 0, 0, 1);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(2, status->country.armies);
        TS_ASSERT_EQUALS(9, srd->players[0].armies);

        send_cmd(&p1, SR_CMD_PLACE, 0, 0, 5);
        TS_ASSERT_EQUALS(SR_CMD_COUNTRY_STATUS, all_res->command);
        TS_ASSERT_EQUALS(7, status->country.armies);
        TS_ASSERT_EQUALS(4, srd->players[0].armies);

        //Negative armies
        send_cmd(&p1, SR_CMD_PLACE, 0, 0, -1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_ENOUGH_ARMIES, err->error);

        //Can't place on other people's countries
        send_cmd(&p1, SR_CMD_PLACE, 0, 1, 1);
        TS_ASSERT_EQUALS(SR_CMD_ERROR, err->command);
        TS_ASSERT_EQUALS(SR_ERR_NOT_OWNER, err->error);
    }

    void clear_board() {
        //This is a completely invalid state, but useful
        for(int i=0; i<SR_NUM_COUNRIES; i++) {
            srd->status.countries[i].owner = 0;
            srd->status.countries[i].armies = 1;
        }
        for(int i=0; i<3; i++) {
            srd->players[i].countries_held = 1;
            srd->players[i].armies = 0;
        }
        reset_mocks();
    }

    void test_produce_armies_country() {
        clear_board();
        srd->status.countries[SR_EASTERN_US].owner = 1;
        srd->status.countries[SR_VENEZUELA].owner = 1;
        srd->status.countries[SR_NORTH_AFRICA].owner = 1;
        srd->status.countries[SR_ICELAND].owner = 1;
        srd->status.countries[SR_JAPAN].owner = 1;
        srd->status.countries[SR_NEW_GUINEA].owner = 1;

        srd->players[1].countries_held = 11;
        srd->players[2].countries_held = 12;
        produce_armies(&game);

        TS_ASSERT_EQUALS(3, cmd_res(0)->armies);
        TS_ASSERT_EQUALS(3, cmd_res(1)->armies);
        TS_ASSERT_EQUALS(4, cmd_res(2)->armies);

        for(int i=0; i<3; i++) {
            srd->players[i].countries_held = 30 + i;
            srd->players[i].armies = 0;
        }
        reset_mocks();
        produce_armies(&game);

        TS_ASSERT_EQUALS(10, cmd_res(0)->armies);
        TS_ASSERT_EQUALS(10, cmd_res(1)->armies);
        TS_ASSERT_EQUALS(10, cmd_res(2)->armies);
    }

    void test_produce_min_max_armies() {
        clear_board();
        srd->players[0].armies = 250;
        srd->players[2].armies = 0;
        srd->players[0].countries_held = 40;
        srd->players[2].countries_held = 0;
        produce_armies(&game);
        TS_ASSERT_EQUALS(255, srd->players[0].armies);
        TS_ASSERT_EQUALS(255, cmd_res(0)->armies);
        TS_ASSERT_EQUALS(0,   srd->players[2].armies);
        TS_ASSERT_EQUALS(0,   cmd_res(2)->armies);
    }

    void test_produce_armies_continent() {
        SR_Game_Status *s = &srd->status;
        clear_board();
        s->countries[SR_EASTERN_US].owner = 1;
        s->countries[SR_VENEZUELA].owner = 1;
        s->countries[SR_NEW_GUINEA].owner = 1;

        s->countries[SR_ICELAND].owner = 1;
        s->countries[SR_SOUTHERN_EUROPE].owner = 1;
        s->countries[SR_UKRAINE].owner = 1;
        s->countries[SR_SCANDINAVIA].owner = 1;
        s->countries[SR_GREAT_BRITAIN].owner = 1;
        s->countries[SR_WESTERN_EUROPE].owner = 1;
        s->countries[SR_NORTHERN_EUROPE].owner = 1;

        s->countries[SR_EGYPT].owner = 2;
        s->countries[SR_CONGO].owner = 2;
        s->countries[SR_MADAGASCAR].owner = 2;
        s->countries[SR_SOUTH_AFRICA].owner = 2;
        s->countries[SR_EAST_AFRICA].owner = 2;
        s->countries[SR_NORTH_AFRICA].owner = 2;

        produce_armies(&game);
        TS_ASSERT_EQUALS(10, cmd_res(0)->armies); //3 + Asia (7) = 10
        TS_ASSERT_EQUALS( 8, cmd_res(1)->armies); //3 + Europe(5) = 8
        TS_ASSERT_EQUALS( 6, cmd_res(2)->armies); //3 + Africa(3) = 6

        clear_board();
        s->countries[SR_NORTH_AFRICA].owner = 1;
        s->countries[SR_ICELAND].owner = 1;
        s->countries[SR_JAPAN].owner = 1;

        s->countries[SR_BRAZIL].owner = 1;
        s->countries[SR_VENEZUELA].owner = 1;
        s->countries[SR_ARGENTINA].owner = 1;
        s->countries[SR_PERU].owner = 1;

        s->countries[SR_NEW_GUINEA].owner = 2;
        s->countries[SR_INDONESIA].owner = 2;
        s->countries[SR_WESTERN_AUSTRALIA].owner = 2;
        s->countries[SR_EASTERN_AUSTRALIA].owner = 2;

        produce_armies(&game);
        TS_ASSERT_EQUALS(8, cmd_res(0)->armies); //3 + N. Am(5) = 8
        TS_ASSERT_EQUALS(5, cmd_res(1)->armies); //3 + S. Am(2) = 5
        TS_ASSERT_EQUALS(5, cmd_res(2)->armies); //3 + Austr(2) = 5
    }

    void test_conquer_country() {
        SR_Move_Result *mv = (SR_Move_Result*)all_res;

        //Attacker wins
        setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
        setup_country(SR_NEW_GUINEA,        1, 1);
        set_random(5,5,5,1);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 3);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(0, mv->country1.owner);
        TS_ASSERT_EQUALS(6, mv->country1.armies);
        TS_ASSERT_EQUALS(0, mv->country2.owner);
        TS_ASSERT_EQUALS(3, mv->country2.armies);

        //Attacker wins
        setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
        setup_country(SR_NEW_GUINEA,        1, 2);
        set_random(5,5,4,4);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 2);
        TS_ASSERT_EQUALS((int)SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(0, mv->country1.owner);
        TS_ASSERT_EQUALS(7, mv->country1.armies);
        TS_ASSERT_EQUALS(0, mv->country2.owner);
        TS_ASSERT_EQUALS(2, mv->country2.armies);

        //Attacker wins
        setup_country(SR_WESTERN_AUSTRALIA, 0, 9);
        setup_country(SR_NEW_GUINEA,        1, 1);
        set_random(5,5,5,1);
        send_cmd(&p1, SR_CMD_ATTACK, SR_WESTERN_AUSTRALIA, SR_NEW_GUINEA, 8);
        TS_ASSERT_EQUALS(SR_CMD_ATTACK_RESULT, mv->command.command);
        TS_ASSERT_EQUALS(0, mv->country1.owner);
        TS_ASSERT_EQUALS(1, mv->country1.armies);
        TS_ASSERT_EQUALS(0, mv->country2.owner);
        TS_ASSERT_EQUALS(8, mv->country2.armies);
    }

    Game game;
    Player p1;
    Player p2;
    Player p3;
    SpeedRiskData *srd;
    SR_Command cmd;
    SR_Command *plr_res;
    SR_Command *all_res;
    SR_Error *err;
};

#endif
