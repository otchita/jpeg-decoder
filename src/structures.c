#include "idct.h"
#include "structures.h"

/*
initializes [[Y1, Y2, Cb, Cr], ...] in the case of a horizontal subsampling,
each time with Y1, etc a 8x8 elements long list
*/
int16_t ***init_decoded_blocks_3D(uint32_t nb_blocks, enum color_mode cm,
                                  uint8_t sf_Y_h, uint8_t sf_Y_v) {
  int16_t ***decoded_blocks =
      (int16_t ***)malloc(nb_blocks * sizeof(*decoded_blocks));
  for (size_t i = 0; i < nb_blocks; i++) {
    decoded_blocks[i] = (int16_t **)malloc((cm + sf_Y_h * sf_Y_v - 1) *
                                           sizeof(*decoded_blocks[i]));
    for (size_t j = 0; j < (cm + sf_Y_h * sf_Y_v - 1); j++) {
      int16_t *block = (int16_t *)malloc(64 * sizeof(*decoded_blocks[i][j]));
      for (size_t k = 0; k < 64; k++) {
        block[k] = 0;
      }
      decoded_blocks[i][j] = block;
    }
  }
  return decoded_blocks;
}

int16_t ***init_decoded_blocks_3D2(uint32_t nb_blocks, enum color_mode cm,
                                   uint8_t sf_Y_h, uint8_t sf_Y_v,
                                   uint8_t sf_Cb_h, uint8_t sf_Cb_v,
                                   uint8_t sf_Cr_h, uint8_t sf_Cr_v) {
  int16_t ***decoded_blocks =
      (int16_t ***)malloc(nb_blocks * sizeof(*decoded_blocks));
  for (size_t i = 0; i < nb_blocks; i++) {
    decoded_blocks[i] = (int16_t **)malloc(
        (sf_Y_h * sf_Y_v +
         (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v)) *
        sizeof(*decoded_blocks[i]));
    for (int j = 0;
         j < (sf_Y_h * sf_Y_v +
              (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
         j++) {
      int16_t *block = (int16_t *)malloc(64 * sizeof(*decoded_blocks[i][j]));
      for (size_t k = 0; k < 64; k++) {
        block[k] = 0;
      }
      decoded_blocks[i][j] = block;
    }
  }
  return decoded_blocks;
}

/*
frees a table initialized with the latest function
*/
void free_decoded_blocks_3D(int16_t ***decoded_blocks, uint32_t nb_blocks,
                            enum color_mode cm, uint8_t sf_Y_h,
                            uint8_t sf_Y_v) {
  for (size_t i = 0; i < nb_blocks; i++) {
    for (size_t j = 0; j < (cm + sf_Y_h * sf_Y_v - 1); j++) {
      free(decoded_blocks[i][j]);
    }
    free(decoded_blocks[i]);
  }
  free(decoded_blocks);
}

void free_decoded_blocks_3D2(int16_t ***decoded_blocks, uint32_t nb_blocks,
                             enum color_mode cm, uint8_t sf_Y_h, uint8_t sf_Y_v,
                             uint8_t sf_Cb_h, uint8_t sf_Cb_v, uint8_t sf_Cr_h,
                             uint8_t sf_Cr_v) {
  for (size_t i = 0; i < nb_blocks; i++) {
    for (int j = 0;
         j < (sf_Y_h * sf_Y_v +
              (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
         j++) {
      free(decoded_blocks[i][j]);
    }
    free(decoded_blocks[i]);
  }
  free(decoded_blocks);
}

/*
creates [[Y, Cb, Cr], ...                           ]
  with Y, Cb and Cr MCUs of 8x8 elements without subsampling, 16x8 with
horizontal,
  8x16 with vertical, and 16x16 with both
*/
uint8_t ***init_MCU_3D(uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h,
                       uint8_t sf_Y_v) {
  uint8_t ***MCUS = (uint8_t ***)malloc(nb_MCU * sizeof(*MCUS));
  for (size_t i = 0; i < nb_MCU; i++) {
    MCUS[i] = (uint8_t **)malloc(cm * sizeof(*MCUS[i]));
    for (size_t j = 0; j < cm; j++) {
      uint8_t *component_MCU =
          (uint8_t *)malloc(sf_Y_h * sf_Y_v * 64 * sizeof(*MCUS[i][j]));
      MCUS[i][j] = component_MCU;
    }
  }
  return MCUS;
}

/*
frees a table initialized with the latest function
*/
void free_MCU_3D(uint8_t ***MCUs, uint32_t nb_blocks, enum color_mode cm) {
  free_result_3D(MCUs, nb_blocks, cm, 1, 1);
}

uint8_t ***init_result_3D(uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h,
                          uint8_t sf_Y_v) {
  uint8_t ***results = (uint8_t ***)malloc(nb_MCU * sizeof(*results));
  for (size_t i = 0; i < nb_MCU; i++) {
    results[i] =
        (uint8_t **)malloc((cm + sf_Y_h * sf_Y_v - 1) * sizeof(*results[i]));
    for (size_t j = 0; j < (cm + sf_Y_h * sf_Y_v - 1); j++) {
      results[i][j] = (uint8_t *)malloc(64 * sizeof(uint8_t));
    }
  }
  return results;
}

uint8_t ***init_result_3D2(uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h,
                           uint8_t sf_Y_v, uint8_t sf_Cb_h, uint8_t sf_Cb_v,
                           uint8_t sf_Cr_h, uint8_t sf_Cr_v) {
  uint8_t ***results = (uint8_t ***)malloc(nb_MCU * sizeof(*results));
  for (size_t i = 0; i < nb_MCU; i++) {
    results[i] = (uint8_t **)malloc(
        (sf_Y_h * sf_Y_v +
         (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v)) *
        sizeof(*results[i]));
    for (int j = 0;
         j < (sf_Y_h * sf_Y_v +
              (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
         j++) {
      results[i][j] = (uint8_t *)malloc(64 * sizeof(uint8_t));
    }
  }
  return results;
}

/*
frees a table initialized with the latest function
*/
void free_result_3D(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                    uint8_t sf_Y_h, uint8_t sf_Y_v) {

  for (size_t i = 0; i < nb_MCU; i++) {
    for (size_t j = 0; j < (cm + sf_Y_h * sf_Y_v - 1); j++) {
      free(result[i][j]);
    }
    free(result[i]);
  }
  free(result);
}

void free_result_3D2(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                     uint8_t sf_Y_h, uint8_t sf_Y_v, uint8_t sf_Cb_h,
                     uint8_t sf_Cb_v, uint8_t sf_Cr_h, uint8_t sf_Cr_v) {

  for (size_t i = 0; i < nb_MCU; i++) {
    for (int j = 0;
         j < (sf_Y_h * sf_Y_v +
              (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
         j++) {
      free(result[i][j]);
    }
    free(result[i]);
  }
  free(result);
}

float *init_tab_cucl() {
  float *tab = (float *)malloc(64 * sizeof(*tab));
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      tab[8 * i + j] =
          (i == 0 && j == 0) ? 0.5 : (i == 0 || j == 0) ? 1 / sqrt(2) : 1;
    }
  }
  return tab;
}

float **init_tab_idct() {
  float *tab_cucl = init_tab_cucl();
  float **result = (float **)malloc(65 * sizeof(*result));
  result[64] = tab_cucl;
  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      result[8 * y + x] = (float *)malloc(64 * sizeof(float));
      for (size_t u = 0; u < 8; u++) {
        for (size_t l = 0; l < 8; l++) {
          result[8 * y + x][8 * u + l] = tab_cucl[8 * u + l] *
                                         cos((2 * x + 1) * l * M_PI / 16) *
                                         cos((2 * y + 1) * u * M_PI / 16);
        }
      }
    }
  }
  return result;
}

void free_tab_idct(float **tab_idct_const) {
  for (size_t i = 0; i < 65; i++) {
    free(tab_idct_const[i]);
  }
  free(tab_idct_const);
}
