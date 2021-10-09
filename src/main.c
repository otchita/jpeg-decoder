#include "main.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitstream.h"
#include "color.h"
#include "debug.h"
#include "decompress.h"
#include "huffman.h"
#include "idct.h"
#include "image_maker.h"
#include "iqzz.h"
#include "jpeg_reader.h"
#include "structures.h"
#include "upsampling.h"
/*
returns the output file name
*/
char *filename_out(const char *filename, enum color_mode cm) {
  uint8_t i = 0;
  do {
    i++;
  } while (filename[i] != '.');
  char *nameout = (char *)malloc((i + 5) * sizeof(char));
  nameout[i + 1] = 'p';
  nameout[i + 2] = (cm == bw) ? 'g' : 'p';
  nameout[i + 3] = 'm';
  nameout[i + 4] = '\0';
  for (size_t j = 0; j < i + 1; j++) {
    nameout[j] = filename[j];
  }
  return nameout;
}

int main(int argc, char **argv) {
  if (argc < 2 || argc >= 3) {
    /* Si y'a pas au moins un argument en ligne de commandes, on
     * boude. */
    fprintf(stderr, "Usage: %s <jpeg_file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  // we get the filename back
  const char *filename = argv[1];
  // we create a describer object
  struct jpeg_desc *jdesc = read_jpeg(filename);
  // print_jdesc(jdesc);
  // we create the bitstream of the image data
  struct bitstream *stream = get_bitstream(jdesc);
  /*******************************SETTINGS*************************************/

  // is the image in black & white or is it colored ?
  enum color_mode cm = (get_nb_components(jdesc) == 1) ? bw : colored;
  // what is the output filename ?
  char *ppgm_file = filename_out(filename, cm);

  // how many 8x8 blocks in the image ?
  uint32_t taille_h = get_image_size(jdesc, DIR_H);
  uint32_t taille_v = get_image_size(jdesc, DIR_V);

  uint32_t nb_blocks_h = ceil((float)taille_h / 8);
  uint32_t nb_blocks_v = ceil((float)taille_v / 8);
  // uint32_t nb_blocks = nb_blocks_v * nb_blocks_h;
  // init if no subsampling
  uint32_t nb_MCU = nb_blocks_h;
  uint32_t nb_MCU_h = nb_blocks_h;
  uint32_t nb_MCU_v = nb_blocks_v;

  // sampling part
  uint8_t id_Y = get_frame_component_id(jdesc, 0) - 1;
  uint8_t sf_Y_h = get_frame_component_sampling_factor(jdesc, DIR_H, id_Y);
  uint8_t sf_Y_v = get_frame_component_sampling_factor(jdesc, DIR_V, id_Y);
  // init if no subsampling
  uint8_t sf_Cb_h = 1;
  uint8_t sf_Cb_v = 1;
  uint8_t sf_Cr_h = 1;
  uint8_t sf_Cr_v = 1;
  if (cm == colored) {
    uint8_t id_Cb = get_frame_component_id(jdesc, 1) - 1;
    uint8_t id_Cr = get_frame_component_id(jdesc, 2) - 1;
    sf_Cb_h = get_frame_component_sampling_factor(jdesc, DIR_H, id_Cb);
    sf_Cb_v = get_frame_component_sampling_factor(jdesc, DIR_V, id_Cb);
    sf_Cr_h = get_frame_component_sampling_factor(jdesc, DIR_H, id_Cr);
    sf_Cr_v = get_frame_component_sampling_factor(jdesc, DIR_V, id_Cr);

    nb_MCU_h = ceil((float)nb_blocks_h / sf_Y_h);
    nb_MCU_v = ceil((float)nb_blocks_v / sf_Y_v);
    nb_MCU = nb_MCU_h /* * nb_MCU_v */;
  }

  /***************************END OF SETTINGS**********************************/

  /*********************************PROCESS************************************/
  header(ppgm_file, cm, taille_h, taille_v);
  int16_t
      precedent[sf_Y_h * sf_Y_v +
                (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v)];
  for (uint8_t j = 0;
       j < (sf_Y_h * sf_Y_v +
            (int)(cm == colored) * (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v));
       j++) {
    precedent[j] = 0;
  }
  float **tab_idct_const = init_tab_idct();
  int16_t ***dbs = init_decoded_blocks_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
  uint8_t ***result = init_result_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
  int16_t *tab_after_quantization = (int16_t *)malloc(64 * sizeof(int16_t));
  int16_t *tab_after_zz = (int16_t *)malloc(64 * sizeof(int16_t));
  for (size_t p = 0; p < nb_MCU_v; p++) {

    for (size_t i = 0; i < nb_MCU; i++) {
      for (size_t j = 0; j < (cm + sf_Y_h * sf_Y_v - 1); j++) {
        for (size_t k = 0; k < 64; k++) {
          dbs[i][j][k] = 0;
        }
      }
    }

    // int16_t ***dbs = init_decoded_blocks_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
    // int16_t ***dbs = init_decoded_blocks_3D2(nb_MCU, cm, sf_Y_h, sf_Y_v,
    // sf_Cb_h,
    //                                          sf_Cb_v, sf_Cr_h, sf_Cb_v);
    // uint8_t ***result = init_result_3D(nb_MCU, cm, sf_Y_h, sf_Y_v);
    // uint8_t ***result = init_result_3D2(nb_MCU, cm, sf_Y_h, sf_Y_v, sf_Cb_h,
    //                                     sf_Cb_v, sf_Cr_h, sf_Cb_v);
    // float **tab_idct_const = init_tab_idct();
    // for (size_t i = 0; i < nb_MCU_v; i++) {
    decode_dcac(stream, dbs, nb_MCU, cm, jdesc, sf_Y_h, sf_Y_v, sf_Cb_h,
                sf_Cb_v, sf_Cr_h, sf_Cb_v, precedent);
    for (uint32_t i = 0; i < nb_MCU; i++) {

      uint8_t indice = 0;
      for (int j = 0; j < sf_Y_h * sf_Y_v +
                              (int)(cm == colored) *
                                  (sf_Cb_h * sf_Cb_v + sf_Cr_h * sf_Cr_v);
           j++) {
        if (j < sf_Y_h * sf_Y_v) {
          indice = 0;
        } else if (j < sf_Y_h * sf_Y_v + sf_Cb_h * sf_Cb_v &&
                   j >= sf_Y_h * sf_Y_v) {
          indice = 1;
        } else {
          indice = 2;
        }
        uint8_t index = get_frame_component_quant_index(jdesc, indice);
        uint8_t *quantization_table = get_quantization_table(jdesc, index);
        quantification_inverse(dbs[i][j], quantization_table,
                               tab_after_quantization);

        zigzag_inverse(tab_after_quantization, tab_after_zz);

        tab_idct(tab_after_zz, tab_idct_const, result[i][j]);

        indice += 1;
      }
    }

    if (cm == bw) {
      draw_MCU_line(result, ppgm_file, cm, taille_h, taille_v, nb_MCU_h,
                    nb_MCU_v, sf_Y_h, sf_Y_v);
      // make_PPM(result, ppgm_file, taille_h, taille_v, nb_blocks_h,
      // nb_blocks_v, sf_Y_h,
      //          sf_Y_v);
      // make_PGM(result, ppgm_file, cm, taille_h, taille_v, nb_blocks_h,
      //          nb_blocks_v);
    } else if (sf_Y_h == 1 && sf_Y_v == 1) {
      uint8_t ***color = RGB(result, nb_MCU, sf_Y_h, sf_Y_v);
      draw_MCU_line(color, ppgm_file, cm, taille_h, taille_v, nb_MCU_h,
                    nb_MCU_v, sf_Y_h, sf_Y_v);
      // make_PPM(color, ppgm_file, cm, taille_h, taille_v, nb_MCU_h, nb_MCU_v,
      // sf_Y_h,
      //          sf_Y_v);
      free_MCU_3D(color, nb_MCU, cm);
    } else {
      uint8_t ***result_upsampling =
          upsample(result, nb_MCU, cm, sf_Y_h, sf_Y_v);
      // uint8_t ***result_upsampling = upsample2(
      //     result, nb_MCU, cm, sf_Y_h, sf_Y_v, sf_Cb_h, sf_Cb_v, sf_Cr_h,
      //     sf_Cr_v);
      uint8_t ***color = RGB(result_upsampling, nb_MCU, sf_Y_h, sf_Y_v);
      draw_MCU_line(color, ppgm_file, cm, taille_h, taille_v, nb_MCU_h,
                    nb_MCU_v, sf_Y_h, sf_Y_v);
      // make_PPM(color, ppgm_file, cm, taille_h, taille_v, nb_MCU_h, nb_MCU_v,
      // sf_Y_h,
      //          sf_Y_v);
      free_MCU_3D(result_upsampling, nb_MCU, cm);
      free_MCU_3D(color, nb_MCU, cm);
    }
  }
  // some cleaning
  free_result_3D(result, nb_MCU, cm, sf_Y_h, sf_Y_v);
  free(tab_after_quantization);
  free(tab_after_zz);
  free_decoded_blocks_3D(dbs, nb_MCU, cm, sf_Y_h, sf_Y_v);
  free_tab_idct(tab_idct_const);
  close_jpeg(jdesc);
  free(ppgm_file);
  // happy ending
  return EXIT_SUCCESS;
}
