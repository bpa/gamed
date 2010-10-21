#ifndef GAMED_LIB_DECK_H
#define GAMED_LIB_DECK_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <gamed/card.h>

#define CARDS_IN_ROOK_DECK 57
#define BLACK  0x10
#define RED    0x20
#define YELLOW 0x40
#define GREEN  0x80

__BEGIN_DECLS

typedef struct deck {
	int cards;
	int max_cards;
	char deck[CARDS_IN_ROOK_DECK];
} Deck;

typedef struct hand {
	int cards;
	int max_cards;
	char deck[15];
} Hand;

void shuffle(Deck *);
 int play_is_valid(Deck *, unsigned char play, unsigned char lead, unsigned char trump);
 int beats(unsigned char card, unsigned char high, unsigned char trump);
bool has_card(Hand *, unsigned char card);

__END_DECLS

#endif
