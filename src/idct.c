#include "idct.h"

/* Vérifie qu'un flottant se trouve entre 0 et 255 et ramène la valeur entre 0
 * et 255 le cas échéant. Convertit ensuite le flottant en entier.
 */
void clampage(float *x) {
  *x = (*x < 0) ? 0 : ((*x > 255) ? 255 : *x);
  *x = (uint8_t)*x;
}

uint8_t idct(uint8_t x, uint8_t y, int16_t *tab, float **tab_idct_const) {
  float s = 0;
  for (uint8_t u = 0; u < 8; u++) {
    for (uint8_t l = 0; l < 8; l++) {
      s += tab_idct_const[8 * y + x][8 * u + l] * tab[8 * l + u];
    }
  }
  s *= 1 / sqrt(16);
  s += 128;
  clampage(&s);
  return s;
}

void tab_idct(int16_t *tableau, float **tab_idct_const, uint8_t *tab) {
  for (size_t i = 0; i < 64; i++) {
    tab[i] = 0;
  }
  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      tab[8 * x + y] = idct(x, y, tableau, tab_idct_const);
    }
  }
}
