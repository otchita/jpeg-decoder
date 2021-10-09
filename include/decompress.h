#ifndef __DECOMPRESS_H__
#define __DECOMPRESS_H__

#include <math.h>
#include <stdint.h>

#include "bitstream.h"
#include "debug.h"
#include "huffman.h"
#include "jpeg_reader.h"
#include "main.h"

extern int16_t value_in_magnitude_class(uint8_t magnitude, uint32_t index);

extern void decode_dc(struct bitstream *stream, struct huff_table *h_table,
                      int16_t *decoded_block, int16_t precedent);

extern void decode_ac(struct bitstream *stream, struct huff_table *h_table,
                      int16_t *decoded_block);

extern void decode_dcac(struct bitstream *stream, int16_t ***decoded_blocks,
                        uint32_t nb_MCUs,
                        enum color_mode cm, struct jpeg_desc *jdesc,
                  			uint8_t sf_Y_h, uint8_t sf_Y_v, uint8_t sf_Cb_h,
                        uint8_t sf_Cb_v, uint8_t sf_Cr_h, uint8_t sf_Cr_v, int16_t *precedent);

#endif
