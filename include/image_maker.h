#ifndef __IMAGE_MAKER_H__
#define __IMAGE_MAKER_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"

extern void header(char *filename, enum color_mode cm, uint32_t taille_h,
            uint32_t taille_v);

extern void make_PGM(uint8_t ***blocks, char *pgm_filename, uint32_t taille_h,
                     uint32_t taille_v, uint32_t nb_MCUs_h, uint32_t nb_MCUs_v);

extern void make_PPM(uint8_t ***MCUs, char *ppm_filename, enum color_mode cm, uint32_t taille_h,
              uint32_t taille_v, uint32_t nb_MCUs_h, uint32_t nb_MCUs_v,
              uint8_t sf_Y_h, uint8_t sf_Y_v);

extern void draw_MCU_line(uint8_t ***MCUs, char *ppm_filename, enum color_mode cm, uint32_t taille_h,
              uint32_t taille_v, uint32_t nb_MCUs_h, uint32_t nb_MCUs_v,
              uint8_t sf_Y_h, uint8_t sf_Y_v);


#endif
