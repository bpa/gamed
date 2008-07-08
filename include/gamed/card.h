#ifndef GAMED_LIB_CARD_H
#define GAMED_LIB_CARD_H

/*
 * Card definitions and macros
 */

#define SPADES   0x10
#define HEARTS   0x20
#define CLUBS    0x40
#define DIAMONDS 0x80

/*
 * Rules for one card beating another are based on leading.
 * This means that a > b does not mean b < a
 * As long as the card lead (current high card) is the second argument, this will work correctly
 */
#define IS_TRUMP(T, card) T & card

#define CARD_BEATS(a, b) (a & b & 0xF0 && ((a & 0x0F) > (b & 0x0F)))

#define CARD_BEATS_TRUMP(a, b, T) ((a & T & 0xF0 && (b ^ T) & 0xF0) || CARD_BEATS(a, b))

#endif
