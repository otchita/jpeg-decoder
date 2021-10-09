#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

#include "structures.h"

extern uint8_t *conversionRGB(uint8_t Y, uint8_t Cb, uint8_t Cr);

extern uint8_t ***RGB(uint8_t ***YCbCr_MCUs, uint32_t nb_MCU, uint8_t sf_Y_h, uint8_t sf_Y_v);

#endif
