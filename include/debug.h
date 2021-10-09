#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "jpeg_reader.h"

extern void print_decoded_dcac_block(int16_t *block, size_t index);
extern void print_result(uint8_t *block, uint32_t index);
extern void print_post_qt(int16_t *block, uint32_t index);
extern void print_idct(uint8_t *tab_apres_idct);
extern void print_zz(int16_t *tab_apres_zz);
extern void print_iq(int16_t *tab_apres_quantif);
extern void print_jdesc(struct jpeg_desc *jdesc);

#endif
