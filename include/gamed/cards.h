#ifndef GAMED_LIB_CARDS_H
#define GAMED_LIB_CARDS_H

typedef struct s_card {
    unsigned suit : 4;
    unsigned number : 5;
} Card;

typedef struct s_deck {
    int cards;
    unsigned char card;
} Deck;

void shuffle(Deck *);

#define draw_card(deck, card) \
    deck.cards--; \
    card = deck.card[deck.cards];

#define add_card(deck, card) \
    deck.card[deck.cards] = card; \
    deck.cards++;

#define is_trump(T, card) T & card
#define card_beats(one, two) \
    ((0xF0 & one & two) && (one & 0x0F > two & 0x0F))
#define card_beats_w_trump(T, one, two) \
    (card_beats(one, two) || ( T & one && !(T & two)))

#define has_card(deck, card, tmp, iterator) \
    tmp = (unsigned char*)&deck->card; \
    for(iterator=0; iterator<deck->cards; iterator++) { \
        

#endif
