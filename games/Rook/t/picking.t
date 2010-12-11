use Test;
ok(1);
done_testing;

=begin END
#include <stdlib.h>
#include <stdio.h>

#include <gtest/gtest.h>
#include <Rook/test.h>
#include <test/server.h>
#include <stdint.h>

unsigned char dealt_cards[] = { 0xF0, 0x81, 0x8E, 0x8D, 0x8C, 0x8B, 0x8A,
                                            0x49, 0x47, 0x45, 0x28, 0x26,
                                      0x1C, 0x1A, 0x19, 0x17, 0x16, 0x15 };

class RookPickingTrumpTest: public ::testing::Test {
public:

/*
	void SetUp()    {
        game.playing = 0;
        game_init (&game);
        rook = (Rook*)game.data;
        rook->state = ROOK_WAITING_FOR_PLAYERS;
        ASSERT_TRUE(player_join(&game, &pN));
        ASSERT_TRUE(player_join(&game, &pE));
        ASSERT_TRUE(player_join(&game, &pS));
        ASSERT_TRUE(player_join(&game, &pW));
        ASSERT_EQ(ROOK_BIDDING, rook->state);
        rook->state = ROOK_PICKING_TRUMP;
        rook->bid = 150;
        rook->bidder = pS.in_game_id;
        Deck *hand = (Deck*)&rook->players[pS.in_game_id].hand;
        hand->cards = 15;
        unsigned char *cards = &hand->card;
        strncpy((char*)cards, (char*)&dealt_cards[0], 15);

        err = (RookError*)&mock_plr_buff[0];
        plr_res = (RookCommand*)&mock_plr_buff[0];
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

    #define setup_trump(playerN, a,b,c,d) \
        rook->trump = '\0'; \
        card = '\0'; \
        if (a) { card |= 0x80; } \
        if (b) { card |= 0x40; } \
        if (c) { card |= 0x20; } \
        if (d) { card |= 0x10; } \
        reset_mocks(); \
        cmd.command = ROOK_CMD_CHOOSE_TRUMP; \
        cmd.card = card; \
        handle_request(&game, &playerN, (char*)&cmd, 4); \

    #define invalid_trump(playerN, a,b,c,d) \
        setup_trump(playerN, a,b,c,d) \
        ASSERT_EQ(ROOK_CMD_ERROR, plr_res->command); \
        ASSERT_EQ(ROOK_ERR_INVALID_TRUMP, err->error);

    #define valid_trump(playerN, a,b,c,d) \
        setup_trump(playerN, a,b,c,d) \
        ASSERT_EQ(ROOK_CMD_CHOOSE_TRUMP, all_res->command); \
        ASSERT_EQ(playerN.in_game_id, all_res->player); \
        ASSERT_EQ(card, all_res->card); \
        ASSERT_EQ(card, rook->trump);

    #define invalid_pick(playerN, widow) \
        reset_mocks(); \
        handle_request(&game, &playerN, (char*)&widow[0], 6); \
        ASSERT_EQ(ROOK_CMD_NOP,   all_res->command); \
        ASSERT_EQ(ROOK_CMD_ERROR, plr_res->command); \
        ASSERT_EQ(ROOK_ERR_DONT_HOLD_CARD, err->error);
*/
    uint8_t card;
    Game game;
    Player pN;
    Player pE;
    Player pS;
    Player pW;
    RookData *rook;
    RookCommand cmd;
    RookError *err;
    RookCommand *plr_res;
    RookCommand *all_res;
};

/*
void xtest_trump_picks() {
    invalid_trump(pS, 0,0,0,0);
      valid_trump(pS, 0,0,0,1);
      valid_trump(pS, 0,0,1,0);
    invalid_trump(pS, 0,0,1,1);
      valid_trump(pS, 0,1,0,0);
    invalid_trump(pS, 0,1,0,1);
    invalid_trump(pS, 0,1,1,0);
    invalid_trump(pS, 0,1,1,1);
      valid_trump(pS, 1,0,0,0);
    invalid_trump(pS, 1,0,0,1);
    invalid_trump(pS, 1,0,1,0);
    invalid_trump(pS, 1,0,1,1);
    invalid_trump(pS, 1,1,0,0);
    invalid_trump(pS, 1,1,0,1);
    invalid_trump(pS, 1,1,1,0);
    invalid_trump(pS, 1,1,1,1);
}

void xtest_one_trump_pick() {
    valid_trump(pS, 0,0,0,1);
    cmd.card = 0x20;
    handle_request(&game, &pS, (char*)&cmd, 4); \
    ASSERT_EQ(ROOK_CMD_ERROR, plr_res->command); \
    ASSERT_EQ(ROOK_ERR_TRUMP_CHOSEN, err->error);
}

void xtest_invalid_picks() {
    // Not real cards
    char bad1[] = {ROOK_CMD_CHOOSE_WIDOW, 0xFF, 0x82, 0x83, 0x84, 0x71 };
    invalid_pick(pS, bad1);

    // Last one isn't a real card
    char bad2[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x25 };
    invalid_pick(pS, bad2);

    // Last one isn't a real card
    char bad3[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x00 };
    invalid_pick(pS, bad3);

    // Can't give two of the same
    char bad4[] = {ROOK_CMD_CHOOSE_WIDOW, 0x49, 0x47, 0x45, 0x28, 0x28 };
    invalid_pick(pS, bad4);

    // Can't give two of the same
    char bad5[] = {ROOK_CMD_CHOOSE_WIDOW, 0xF0, 0x47, 0x45, 0x28, 0xF0 };
    invalid_pick(pS, bad5);
}

void xtest_pick_widow_then_trump() {
    TS_FAIL("Add tests to this function");
}

void xtest_pick_trump_then_widow() {
    TS_FAIL("Add tests to this function");
}
*/
