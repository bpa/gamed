#include <gtest/gtest.h>
#include <Rook/rook.h>
#include <test/server.h>

class RookBiddingTest: public ::testing::Test {
public:

/*
	void SetUp()    {
		game = create_instance(&SpeedRisk);
        rook = (RookData*)game.data;
        plr_res = (RookCommand*)&mock_plr_buff[0];
        all_res = (RookCommand*)&mock_all_buff[0];
        ASSERT_TRUE(player_join(&game, &pN));
        ASSERT_TRUE(player_join(&game, &pE));
        ASSERT_TRUE(player_join(&game, &pS));
        ASSERT_TRUE(player_join(&game, &pW));
        ASSERT_EQ(&ROOK_BIDDING, rook->state);
        plr_res = (RookCommand*)&mock_plr_buff[0];
        err = (RookError*)&mock_plr_buff[0];
        all_res = (RookCommand*)&mock_all_buff[0];
    }

	void TearDown() {
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

*/

    #define pass(playerN) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_PASS; \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        ASSERT_EQ(playerN.in_game_id, all_res->player); \
        ASSERT_EQ(ROOK_CMD_PASS, all_res->command);

    #define bid(playerN, player_bid) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_BID; \
        cmd.score = player_bid; \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        ASSERT_EQ(ROOK_CMD_BID, all_res->command); \
        ASSERT_EQ(playerN.in_game_id, all_res->player); \
        ASSERT_EQ(playerN.in_game_id, rook->bidder); \
        ASSERT_EQ(player_bid, all_res->score); \
        ASSERT_EQ(player_bid, rook->bid);

    #define invalid_bid(playerN, bid, error_msg) \
        reset_mocks(); \
        cmd.command = ROOK_CMD_BID; \
        cmd.score = bid; \
        htons(cmd.score); \
        handle_request(&game, &playerN, (char*)&cmd, 4); \
        ASSERT_EQ(ROOK_CMD_NOP,   all_res->command); \
        ASSERT_EQ(ROOK_CMD_ERROR, plr_res->command); \
        ASSERT_EQ(error_msg, err->error);
    Game game;
    Player pN;
    Player pE;
    Player pS;
    Player pW;
    Game *rook;
    RookCommand cmd;
    RookError *err;
    RookCommand *plr_res;
    RookCommand *all_res;
};

/*

void xtest_normal() {
    pass(pN);
    pass(pE);
    bid(pS, 120);
    bid(pW, 140);
    bid(pS, 150);
    pass(pW);

    ASSERT_EQ(ROOK_PICKING_TRUMP, rook->state);
    ASSERT_EQ(150,                rook->bid);
    ASSERT_EQ(pS.in_game_id,      rook->bidder);
    ASSERT_EQ(pS.in_game_id,      rook->current_player);
    all_res = (RookCommand*)&mock_all_buff[1];
    ASSERT_EQ(ROOK_CMD_PICKING_TRUMP, all_res->command);
    ASSERT_EQ(pS.in_game_id, all_res->player);
    ASSERT_EQ(150,           all_res->score);
}

void xtest_max_bid() {
    bid(pN, 100);
    bid(pE, 200);
    ASSERT_EQ(ROOK_PICKING_TRUMP, rook->state);
    ASSERT_EQ(200,                rook->bid);
    ASSERT_EQ(pE.in_game_id,      rook->bidder);
    ASSERT_EQ(pE.in_game_id,      rook->current_player);
    all_res = (RookCommand*)&mock_all_buff[1];
    ASSERT_EQ(ROOK_CMD_PICKING_TRUMP, all_res->command);
    ASSERT_EQ(pE.in_game_id, all_res->player);
    ASSERT_EQ(200,           all_res->score);
}

void xtest_invalid_bids() {
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
    ASSERT_EQ(pE.in_game_id, rook->current_player);
    invalid_bid(pN, 120, ROOK_ERR_NOT_YOUR_TURN);
    invalid_bid(pE, 105, ROOK_ERR_BID_TOO_LOW);
}

*/
