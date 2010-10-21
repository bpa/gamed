#include "deck.h"

void shuffle(Deck *deck) {
}

int play_is_valid(Deck *deck, unsigned char play, unsigned char lead, unsigned char trump) {
	unsigned char suits = '\0';
	int have_card = 0;
	int i;
	for (i=0; i<deck->cards; i++) {
		suits |= deck->deck[i];
		if (deck->deck[i] == play) have_card = 1;
	}
	if (have_card) {
		return (play & lead & 0xF0) || !(suits & lead & 0xF0);
	}
	return 0;
}

int beats(unsigned char card, unsigned char high, unsigned char trump) {
	return 1;
}
