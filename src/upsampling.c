#include "main.h"
#include "upsampling.h"

/* returns  the list of the 2 blocks constructed from a horizontal MCU
_________________________
| 1  1  2  2|3  3  4  4 |
|  block 1  |  block 2  |
|           |           |
|___________|___________|
*/
uint8_t **upsample_MCU_h(uint8_t *h_mcu) {
  uint8_t **both_blocks = (uint8_t **)malloc(2 * sizeof(*both_blocks));
  uint8_t *left_block = (uint8_t *)malloc(64 * sizeof(*left_block));
  uint8_t *right_block = (uint8_t *)malloc(64 * sizeof(*right_block));

  for (size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 8; i++) {
      left_block[8 * j + i] = h_mcu[(16 * j + i) / 2];
    }
  }

  for (size_t j = 0; j < 8; j++)
    for (size_t i = 0; i < 8; i++) {
      right_block[8 * j + i] = h_mcu[(16 * j + 8 + i) / 2];
    }

  both_blocks[0] = left_block;
  both_blocks[1] = right_block;
  return both_blocks;
}

uint8_t **upsample_MCU_h2(uint8_t *h_mcu, uint8_t length) {
  uint8_t **blocks = (uint8_t **)malloc(length * sizeof(*blocks));
  for (size_t k = 0; k < length; k++) {
    blocks[k] = (uint8_t *)malloc(64 * sizeof(*blocks[k]));
  }
  for (size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 8; i++) {
      for (size_t k = 0; k < length; k++) {
        uint8_t which = (length * i + k) / 8;
        blocks[which][8 * j + (length * i + k) % 8] = h_mcu[8 * j + i];
      }
    }
  }
  return blocks;
}

/* returns the list of the 2 blocks constructed from a vertical MCU
   ___________
  |1  2  3  4|
  |1  2  3  4|
  | block 1  |
  |__________|
  |          |
  | block 2  |
  |          |
  |__________|
*/
uint8_t **upsample_MCU_v(uint8_t *v_mcu) {
  uint8_t **both_blocks = (uint8_t **)malloc(2 * sizeof(*both_blocks));
  uint8_t *high_block = (uint8_t *)malloc(64 * sizeof(*high_block));
  uint8_t *low_block = (uint8_t *)malloc(64 * sizeof(*low_block));

  for (size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 8; i++) {
      high_block[8 * j + i] = v_mcu[8 * (j / 2) + i];
    }
  }

  for (size_t j = 0; j < 8; j++)
    for (size_t i = 0; i < 8; i++) {
      low_block[8 * j + i] = v_mcu[8 * ((j / 2) + 4) + i];
    }

  both_blocks[0] = high_block;
  both_blocks[1] = low_block;
  return both_blocks;
}

uint8_t **upsample_MCU_v2(uint8_t *v_mcu, uint8_t height) {
  uint8_t **blocks = (uint8_t **)malloc(height * sizeof(*blocks));
  for (size_t k = 0; k < height; k++) {
    blocks[k] = (uint8_t *)malloc(64 * sizeof(*blocks[k]));
  }
  for (size_t j = 0; j < 8; j++) {
    for (size_t i = 0; i < 8; i++) {
      for (size_t k = 0; k < height; k++) {
        uint8_t which = (height * j + k) / 8;
        blocks[which][(height * 8 * j + 8 * k + i) % 64] = v_mcu[8 * j + i];
      }
    }
  }

  return blocks;
}

/* returns the list of the 4 blocks constructed from a 2x2 MCU
_________________________
|1  1  2  2 |3  3  4  4 |
|1  1  2  2 |3  3  4  4 |
| block 1   |  block 2  |
|___________|___________|
|           |           |
|  block 3  |  block 4  |
|           |           |
|___________|___________|
*/
uint8_t **upsample_MCU_hv(uint8_t *hv_mcu) {
  uint8_t **four_blocks = (uint8_t **)malloc(4 * sizeof(*four_blocks));
  uint8_t **h_blocks = upsample_MCU_h(hv_mcu);
  uint8_t **left_blocks = upsample_MCU_v(h_blocks[0]);
  uint8_t **right_blocks = upsample_MCU_v(h_blocks[1]);
  four_blocks[0] = left_blocks[0];
  four_blocks[1] = right_blocks[0];
  four_blocks[2] = left_blocks[1];
  four_blocks[3] = right_blocks[1];
  free(h_blocks[0]);
  free(h_blocks[1]);
  free(h_blocks);
  free(left_blocks);
  free(right_blocks);
  return four_blocks;
}

uint8_t **upsample_MCU_hv2(uint8_t *hv_mcu, uint8_t length, uint8_t height) {
  uint8_t **blocks = (uint8_t **)malloc(length * height * sizeof(*blocks));
  uint8_t **h_blocks = upsample_MCU_h2(hv_mcu, length);
  // uint8_t **v_blocks = NULL;
  for (size_t i = 0; i < length; i++) {
    uint8_t **v_blocks = upsample_MCU_v2(h_blocks[i], height);
    for (size_t j = 0; j < height; j++) {
      blocks[j * length + i] = v_blocks[j];
    }
    free(h_blocks[i]);
    free(v_blocks);
  }
  free(h_blocks);
  return blocks;
}

uint8_t *join_v(uint8_t *block1, uint8_t *block2) {
  uint8_t *liste_res = (uint8_t *)malloc(128 * sizeof(*liste_res));
  for (uint8_t k = 0; k < 64; k++) {
    liste_res[k] = block1[k];
    liste_res[64 + k] = block2[k];
  }
  // free(block1);
  // free(block2);
  return liste_res;
}

uint8_t *join_v2(uint8_t **v_blocks, uint8_t height) {
  uint8_t *liste_res = (uint8_t *)malloc(height * 64 * sizeof(*liste_res));
  for (size_t j = 0; j < height; j++) {
    for (size_t i = 0; i < 64; i++) {
      liste_res[64 * j + i] = v_blocks[j][i];
    }
    free(v_blocks[j]);
  }

  return liste_res;
}

uint8_t *join_h(uint8_t *block1, uint8_t *block2) {
  uint8_t *liste_res = (uint8_t *)malloc(128 * sizeof(*liste_res));
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      liste_res[16 * i + j] = block1[8 * i + j];
      liste_res[16 * i + j + 8] = block2[8 * i + j];
    }
  }
  return liste_res;
}

uint8_t *join_h2(uint8_t **h_blocks, uint8_t length) {
  uint8_t *liste_res = (uint8_t *)malloc(length * 64 * sizeof(*liste_res));
  for (size_t k = 0; k < length; k++) {
    for (size_t i = 0; i < 8; i++) {
      for (size_t j = 0; j < 8; j++) {
        liste_res[8 * length * i + j + 8 * k] = h_blocks[k][8 * i + j];
      }
    }
  }
  return liste_res;
}

uint8_t *join_hv(uint8_t *block1, uint8_t *block2, uint8_t *block3,
                 uint8_t *block4) {
  uint8_t *liste_res = (uint8_t *)malloc(256 * sizeof(*liste_res));
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      liste_res[16 * i + j] = block1[8 * i + j];
      liste_res[16 * i + j + 8] = block2[8 * i + j];
      liste_res[16 * (i + 8) + j] = block3[8 * i + j];
      liste_res[16 * (i + 8) + j + 8] = block4[8 * i + j];
    }
  }
  return liste_res;
}

uint8_t *join_hv2(uint8_t **blocks, uint8_t length, uint8_t height) {
  uint8_t *liste_res =
      (uint8_t *)malloc(length * height * 64 * sizeof(*liste_res));
  for (size_t h = 0; h < height; h++) {
    for (size_t l = 0; l < length; l++) {
      for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < 8; j++) {
          liste_res[length * 8 * (i + 8 * h) + j + 8 * l] =
              blocks[length * h + l][8 * i + j];
        }
      }
    }
  }
  return liste_res;
}

uint8_t ***upsample(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                    uint8_t sf_Y_h, uint8_t sf_Y_v) {

  uint8_t ***tab = init_MCU_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
  for (uint32_t i = 0; i < nb_MCU; i++) {
    for (uint8_t j = 0; j < cm; j++) {

      // On met les Y du mcu considéré au bon endroit
      if (j == 0) {
        if (sf_Y_v == 2 && !(sf_Y_h == 2)) {
          free(tab[i][j]);
          tab[i][j] = join_v(result[i][j], result[i][j + 1]);
        } else if (sf_Y_h == 2 && !(sf_Y_v == 2)) {
          free(tab[i][j]);
          tab[i][j] = join_h(result[i][j], result[i][j + 1]);
        } else {
          free(tab[i][j]);
          tab[i][j] = join_hv(result[i][j], result[i][j + 1], result[i][j + 2],
                              result[i][j + 3]);
        }
      }

      // On duplique les Cb et les Cr on les met au bon endroit
      else {
        if (sf_Y_v == 2 && !(sf_Y_h == 2)) {
          uint8_t **blocks_CbCr = upsample_MCU_v(result[i][j + 1]);
          free(tab[i][j]);
          tab[i][j] = join_v(blocks_CbCr[0], blocks_CbCr[1]);
          free(blocks_CbCr[0]);
          free(blocks_CbCr[1]);
          free(blocks_CbCr);
        } else if (sf_Y_h == 2 && !(sf_Y_v == 2)) {
          uint8_t **blocks_CbCr = upsample_MCU_h(result[i][j + 1]);
          free(tab[i][j]);
          tab[i][j] = join_h(blocks_CbCr[0], blocks_CbCr[1]);
          free(blocks_CbCr[0]);
          free(blocks_CbCr[1]);
          free(blocks_CbCr);
        } else {
          uint8_t **blocks_CbCr = upsample_MCU_hv(result[i][j + 3]);
          free(tab[i][j]);
          tab[i][j] = join_hv(blocks_CbCr[0], blocks_CbCr[1], blocks_CbCr[2],
                              blocks_CbCr[3]);
          free(blocks_CbCr[0]);
          free(blocks_CbCr[1]);
          free(blocks_CbCr[2]);
          free(blocks_CbCr[3]);
          free(blocks_CbCr);
        }
      }
    }
  }
  return tab;
}

uint8_t ***upsample2(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                     uint8_t sf_Y_h, uint8_t sf_Y_v, uint8_t sf_Cb_h,
                     uint8_t sf_Cb_v, uint8_t sf_Cr_h, uint8_t sf_Cr_v) {

  uint8_t ***tab = init_MCU_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
  uint8_t length, height;
  for (uint32_t i = 0; i < nb_MCU; i++) {
    for (uint8_t j = 0; j < cm; j++) {

      if (j == 0) {
        free(tab[i][j]);
        uint8_t **temp = (uint8_t **)malloc(sf_Y_h * sf_Y_v * sizeof(*temp));
        for (size_t k = 0; k < sf_Y_h * sf_Y_v; k++) {
          temp[k] = result[i][k];
        }
        tab[i][j] = join_hv2(temp, sf_Y_h, sf_Y_v);
        free(temp);
      }

      else {
        free(tab[i][j]);
        length = j == 1 ? sf_Cb_h : sf_Cr_h;
        height = j == 1 ? sf_Cb_v : sf_Cr_v;
        uint8_t **blocks_CbCr = upsample_MCU_hv2(
            result[i][j + sf_Y_h * sf_Y_v - 1], length, height);
        tab[i][j] = join_hv2(blocks_CbCr, length, height);
        for (size_t i = 0; i < length * height; i++) {
          free(blocks_CbCr[i]);
        }
        free(blocks_CbCr);
      }
    }
  }
  return tab;
}
