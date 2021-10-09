#ifndef __IDCT_H__
#define __IDCT_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846

extern void clampage(float *x);

extern uint8_t idct(uint8_t x, uint8_t y, int16_t *tab, float **tab_idct_const);
extern void tab_idct(int16_t *tableau, float **tab_idct_const, uint8_t *tab);

#endif
