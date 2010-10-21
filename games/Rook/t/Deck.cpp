#include <gtest/gtest.h>
#include <Rook/rook.h>
#include <string.h>

#define ASSERT_DECK(deck, max, ncards, arr) \
	{ \
		int i; \
		char expected[] = arr; \
		ASSERT_EQ(deck.max_cards, max); \
		ASSERT_EQ(deck.cards, ncards); \
		for(i=0; i< ncards; i++) { \
			ASSERT_EQ(expected[i], deck.card[i]); \
		} \
	}


class DeckTest: public ::testing::Test {
public:
	void SetUp() {
		deck.cards = 0;
		deck.max_cards = 57;
		hand.cards = 0;
		hand.max_cards = 15;
	}

	Deck deck;
	Hand hand;
};

TEST_F (DeckTest, valid_play) {
	hand.cards = 5;
	hand.deck[0] = RED | 1;
	hand.deck[1] = GREEN | 1;
	hand.deck[2] = GREEN | 2;
	hand.deck[3] = YELLOW | 1;
	hand.deck[4] = YELLOW | 2;
	// Have to have the card to play it
	ASSERT_TRUE(!play_is_valid((Deck*)&hand, RED | 2, RED, RED));
	ASSERT_TRUE( play_is_valid((Deck*)&hand, RED | 1, RED, RED));
	// Must follow suit
	ASSERT_TRUE(!play_is_valid((Deck*)&hand, GREEN | 1, RED, RED));
	ASSERT_TRUE( play_is_valid((Deck*)&hand,   RED | 1, RED, RED));
	// Unless you don't have it
	ASSERT_TRUE( play_is_valid((Deck*)&hand, RED | 1, BLACK, RED));
}

