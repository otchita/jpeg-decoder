#include "decompress.h"

// returns the value corresponding to the couple (magnitude, index)
int16_t value_in_magnitude_class(uint8_t magnitude, uint32_t index) {
  int16_t result;
  result =
      (index >= pow(2, magnitude - 1)) ? index : -pow(2, magnitude) + 1 + index;
  return result;
}

// calculates the first value of the block (DC)
void decode_dc(struct bitstream *stream, struct huff_table *h_table,
               int16_t *decoded_block, int16_t precedent) {
  uint32_t index;
  uint8_t magnitude = next_huffman_value(h_table, stream);
  read_bitstream(stream, magnitude, &index, true);
  decoded_block[0] = precedent + value_in_magnitude_class(magnitude, index);
}

// calculates the 63 last values of the block (AC)
void decode_ac(struct bitstream *stream, struct huff_table *h_table,
               int16_t *decoded_block) {
  uint32_t index;
  uint8_t i = 0;
  while (i < 64) {
    uint8_t read_8bits = next_huffman_value(h_table, stream);
    uint8_t first4bits = read_8bits / 16;
    uint8_t last4bits = read_8bits % 16;
    i += first4bits + 1;
    if (last4bits == 0) {
      // End Of Block case
      if (first4bits == 0) {
        return;
      }
      continue;
    }
    uint8_t magnitude = last4bits;
    read_bitstream(stream, magnitude, &index, true);
    decoded_block[i] = value_in_magnitude_class(magnitude, index);
    if (i == 63) {
      return;
    }
  }
}

void decode_dcac(struct bitstream *stream, int16_t ***decoded_blocks,
                 uint32_t nb_MCUs, enum color_mode cm, struct jpeg_desc *jdesc,
                 uint8_t sf_Y_h, uint8_t sf_Y_v, uint8_t sf_Cb_h,
                 uint8_t sf_Cb_v, uint8_t sf_Cr_h, uint8_t sf_Cr_v,
                 int16_t *precedent) {

  for (uint32_t i = 0; i < nb_MCUs; i++) {
    uint8_t indice = 0;
    uint8_t k;
    for (int32_t j = 0;
         j < (sf_Y_h * sf_Y_v +
              (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
         j++) {
      if (j < sf_Y_h * sf_Y_v) {
        indice = 0;
        k = (j == 0) ? (sf_Y_h * sf_Y_v - 1) : (j - 1);
      } else if (j < sf_Y_h * sf_Y_v + sf_Cb_h * sf_Cb_v &&
                 j >= sf_Y_h * sf_Y_v) {
        indice = 1;
        k = (j == sf_Y_h * sf_Y_v) ? (sf_Y_h * sf_Y_v + sf_Cb_h * sf_Cb_v - 1)
                                   : (j - 1);
      } else {
        indice = 2;
        k = (j == sf_Y_h * sf_Y_v + sf_Cb_h * sf_Cb_v)
                ? (sf_Y_h * sf_Y_v + sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v - 1)
                : (j - 1);
      }
      uint8_t h_table_dc_i =
          get_scan_component_huffman_index(jdesc, DC, indice);
      uint8_t h_table_ac_i =
          get_scan_component_huffman_index(jdesc, AC, indice);
      struct huff_table *h_table_dc =
          get_huffman_table(jdesc, DC, h_table_dc_i);
      struct huff_table *h_table_ac =
          get_huffman_table(jdesc, AC, h_table_ac_i);
      decode_dc(stream, h_table_dc, decoded_blocks[i][j], precedent[k]);
      decode_ac(stream, h_table_ac, decoded_blocks[i][j]);
      precedent[j] = decoded_blocks[i][j][0];
    }
  }
}
