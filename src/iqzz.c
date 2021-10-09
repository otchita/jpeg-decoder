#include "iqzz.h"

// inverse quantification
void quantification_inverse(int16_t *decoded_block,
                            uint8_t *quantification_table, int16_t *tab) {
  for (size_t i = 0; i < 64; i++) {
    tab[i] = decoded_block[i] * quantification_table[i];
  }
}

// inverse ZZ
void zigzag_inverse(int16_t *tableau, int16_t *tab) {
  enum direction { FORWARD, DOWNWARD, ZIG, ZAG };
  uint8_t x = 0;
  uint8_t y = 0;
  enum direction dir = FORWARD;
  for (size_t i = 0; i < 64; i++) {
    tab[(x + y)] = tableau[i];
    switch (dir) {
    case FORWARD:
      x++;
      dir = (y < 8) ? ZIG : ZAG;
      break;
    case ZIG:
      x--;
      y += 8;
      dir = (x == 0 && y < 56) ? DOWNWARD : ((y >= 56) ? FORWARD : ZIG);
      break;
    case ZAG:
      x++;
      y -= 8;
      dir = (y < 8) ? FORWARD : ((x == 7) ? DOWNWARD : ZAG);
      break;
    case DOWNWARD:
      y += 8;
      dir = (x == 0) ? ZAG : ((x == 7) ? ZIG : DOWNWARD);
      break;
    default:
      break;
    }
  }
}
