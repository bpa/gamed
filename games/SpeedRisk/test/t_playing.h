#ifndef SPEED_RISK_PLAYING_TEST_H__
#define SPEED_RISK_PLAYING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <mocks.h>
#include <SpeedRisk/board.h>
#include <SpeedRisk/protocol.h>

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
