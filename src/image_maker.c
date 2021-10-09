#include "image_maker.h"
#include "main.h"

void header(char *filename, enum color_mode cm, uint32_t taille_h,
            uint32_t taille_v) {
  FILE *file = fopen(filename, "wb");
  if (cm == bw) {
    fprintf(file, "P5\n");
  } else {
    fprintf(file, "P6\n");
  }
  fprintf(file, "%u %u\n", taille_h, taille_v);
  fprintf(file, "255\n");
  fclose(file);
}

// creates the PGM file
void make_PGM(uint8_t ***blocks, char *pgm_filename, uint32_t taille_h,
              uint32_t taille_v, uint32_t nb_MCUs_h, uint32_t nb_MCUs_v) {
  FILE *file = fopen(pgm_filename, "wb");
  fprintf(file, "P5\n");
  fprintf(file, "%u %u\n", taille_h, taille_v);
  fprintf(file, "255\n");
  for (size_t v = 0; v < nb_MCUs_v; v++) {
    for (size_t i = 0;
         i < (v == nb_MCUs_v - 1 && taille_v % 8 != 0 ? taille_v % 8 : 8);
         i++) {
      for (size_t h = 0; h < nb_MCUs_h; h++) {
        uint16_t nb_last_px =
            h == nb_MCUs_h - 1 && taille_h % 8 != 0 ? taille_h % 8 : 8;
        for (size_t j = 0; j < nb_last_px; j++) {
          putc(blocks[nb_MCUs_h * v + h][0][8 * i + j], file);
        }
      }
    }
  }
  fclose(file);
}

void make_PPM(uint8_t ***MCUs, char *ppm_filename, enum color_mode cm,
              uint32_t taille_h, uint32_t taille_v, uint32_t nb_MCUs_h,
              uint32_t nb_MCUs_v, uint8_t sf_Y_h, uint8_t sf_Y_v) {

  FILE *file = fopen(ppm_filename, "ab");
  for (size_t v = 0; v < nb_MCUs_v; v++) {
    for (size_t i = 0; i < (v == nb_MCUs_v - 1 && taille_v % (8 * sf_Y_v) != 0
                                ? taille_v % (8 * sf_Y_v)
                                : 8 * sf_Y_v);
         i++) {
      for (size_t h = 0; h < nb_MCUs_h; h++) {
        uint8_t nb_last_px = h == nb_MCUs_h - 1 && taille_h % (8 * sf_Y_h) != 0
                                 ? taille_h % (8 * sf_Y_h)
                                 : 8 * sf_Y_h;
        for (size_t j = 0; j < nb_last_px; j++) {
          for (size_t k = 0; k < cm; k++) {
            putc(MCUs[nb_MCUs_h * v + h][k][sf_Y_h * 8 * i + j], file);
          }
        }
      }
    }
  }
  fclose(file);
}

void draw_MCU_line(uint8_t ***MCUs, char *ppm_filename, enum color_mode cm,
                   uint32_t taille_h, uint32_t taille_v, uint32_t nb_MCUs_h,
                   uint32_t nb_MCUs_v, uint8_t sf_Y_h, uint8_t sf_Y_v) {
  FILE *file = fopen(ppm_filename, "ab");
  for (size_t i = 0; i < (nb_MCUs_v == 1 && taille_v % (8 * sf_Y_v) != 0
                              ? taille_v % (8 * sf_Y_v)
                              : 8 * sf_Y_v);
       i++) {
    for (size_t h = 0; h < nb_MCUs_h; h++) {
      uint8_t nb_last_px = h == nb_MCUs_h - 1 && taille_h % (8 * sf_Y_h) != 0
                               ? taille_h % (8 * sf_Y_h)
                               : 8 * sf_Y_h;
      for (size_t j = 0; j < nb_last_px; j++) {
        for (size_t k = 0; k < cm; k++) {
          putc(MCUs[h][k][sf_Y_h * 8 * i + j], file);
        }
      }
    }
  }
  fclose(file);
}
