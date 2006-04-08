#ifndef ROOK_BIDDING_TEST_H__
#define ROOK_BIDDING_TEST_H__

#include <stdlib.h>
#include <stdio.h>

#include <cxxtest/TestSuite.h>
#include <gamed/game.h>
#include <Rook/game.h>
#include <arpa/inet.h>
#include <mocks.h>

class RookBiddingTest: public CxxTest::TestSuite {
public:

	void setUp()    {
        game.playing = 0;
        game_init (&game);
        rook = (Rook*)game.data;
        rook->state = ROOK_WAITING_FOR_PLAYERS;
        TS_ASSERT(player_join(&game, &pN));
        TS_ASSERT(player_join(&game, &pE));
        TS_ASSERT(player_join(&game, &pS));
        TS_ASSERT(player_join(&game, &pW));
        TS_ASSERT_EQUALS(ROOK_BIDDING, rook->state);
        plr_res = (RookCommand*)&mock_plr_buff[0];
        err = (RookError*)&mock_plr_buff[0];
        all_res = (RookCommand*)&mock_all_buff[0];
    }

	void tearDown() {
        Player *first, *next;
        first = LIST_FIRST(&game.players);
        while (first != NULL) {
            next = LIST_NEXT(first, players);
            LIST_REMOVE(first, players);
            first = next;
        }
        free(game.data);
        game.data = NULL;
    }

    #define pass(playerN) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_PASS; \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        TS_ASSERT_EQUALS(playerN.in_game_id, all_res->player); \
        TS_ASSERT_EQUALS(ROOK_CMD_PASS, all_res->command);

    #define bid(playerN, player_bid) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_BID; \
        cmd.score = player_bid; \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        TS_ASSERT_EQUALS(ROOK_CMD_BID, all_res->command); \
        TS_ASSERT_EQUALS(playerN.in_game_id, all_res->player); \
        TS_ASSERT_EQUALS(playerN.in_game_id, rook->bidder); \
        TS_ASSERT_EQUALS(player_bid, all_res->score); \
        TS_ASSERT_EQUALS(player_bid, rook->bid);

    #define invalid_bid(playerN, bid, error_msg) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_BID; \
        cmd.score = bid; \
        htons(cmd.score); \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        TS_ASSERT_EQUALS(ROOK_CMD_NOP,   all_res->command); \
        TS_ASSERT_EQUALS(ROOK_CMD_ERROR, plr_res->command); \
        TS_ASSERT_EQUALS(error_msg, err->error);

    void test_normal() {
        pass(pN);
        pass(pE);
        bid(pS, 120);
        bid(pW, 140);
        bid(pS, 150);
        pass(pW);

        TS_ASSERT_EQUALS(ROOK_PICKING_TRUMP, rook->state);
        TS_ASSERT_EQUALS(150,                rook->bid);
        TS_ASSERT_EQUALS(pS.in_game_id,      rook->bidder);
        TS_ASSERT_EQUALS(pS.in_game_id,      rook->current_player);
        all_res = (RookCommand*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(ROOK_CMD_PICKING_TRUMP, all_res->command);
        TS_ASSERT_EQUALS(pS.in_game_id, all_res->player);
        TS_ASSERT_EQUALS(150,           all_res->score);
    }

    void test_max_bid() {
        bid(pN, 100);
        bid(pE, 200);
        TS_ASSERT_EQUALS(ROOK_PICKING_TRUMP, rook->state);
        TS_ASSERT_EQUALS(200,                rook->bid);
        TS_ASSERT_EQUALS(pE.in_game_id,      rook->bidder);
        TS_ASSERT_EQUALS(pE.in_game_id,      rook->current_player);
        all_res = (RookCommand*)&mock_all_buff[1];
        TS_ASSERT_EQUALS(ROOK_CMD_PICKING_TRUMP, all_res->command);
        TS_ASSERT_EQUALS(pE.in_game_id, all_res->player);
        TS_ASSERT_EQUALS(200,           all_res->score);
    }

    void test_invalid_bids() {
        int16_t max = -1;
        invalid_bid(pN,  95, ROOK_ERR_BID_TOO_LOW);
        invalid_bid(pN,   0, ROOK_ERR_BID_TOO_LOW);

        invalid_bid(pN, 205, ROOK_ERR_BID_TOO_HIGH);
        invalid_bid(pN, max, ROOK_ERR_BID_TOO_HIGH);

        invalid_bid(pW,  25, ROOK_ERR_NOT_YOUR_TURN);
        invalid_bid(pE, 150, ROOK_ERR_NOT_YOUR_TURN);
        invalid_bid(pS, 155, ROOK_ERR_NOT_YOUR_TURN);

        invalid_bid(pN, 101, ROOK_ERR_INVALID_BID);
        invalid_bid(pN, 104, ROOK_ERR_INVALID_BID);
        invalid_bid(pN, 199, ROOK_ERR_INVALID_BID);

        pass(pN);
        bid(pE, 105);
        bid(pS, 110);
        bid(pW, 115);
        TS_ASSERT_EQUALS(pE.in_game_id, rook->current_player);
        invalid_bid(pN, 120, ROOK_ERR_NOT_YOUR_TURN);
        invalid_bid(pE, 105, ROOK_ERR_BID_TOO_LOW);
    }

    Game game;
    Player pN;
    Player pE;
    Player pS;
    Player pW;
    Rook *rook;
    RookCommand cmd;
    RookError *err;
    RookCommand *plr_res;
    RookCommand *all_res;
};

#endif
