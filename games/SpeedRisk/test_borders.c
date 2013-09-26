#include <stdio.h>
#include "board.h"
#include "boards.h"

extern int8_t border_table[42][42];
extern char classic_borders[42][42];

int main() {
  int i, j;
  for (i=0; i<42; i++) {
    for (j=0; j<42; j++) {
      printf("%i, %i\r", i, j);
      if (border_table[i][j] != classic_borders[i][j]) {
        printf("Differs at (%i,%i) %i != %i\n", i, j, border_table[i][j], classic_borders[i][j]);
      }
    }
  }
  return 0;
}
