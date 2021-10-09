#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#include <stdint.h>
#include <math.h>

#include "main.h"

extern int16_t ***init_decoded_blocks_3D(uint32_t nb_blocks, enum color_mode cm,
                                         uint8_t sf_Y_h, uint8_t sf_Y_v);

extern int16_t ***init_decoded_blocks_3D2(uint32_t nb_blocks, enum color_mode cm,
                                    uint8_t sf_Y_h, uint8_t sf_Y_v,
                                    uint8_t h_Cb_sampling, uint8_t h_Cr_sampling,
                                    uint8_t v_Cb_sampling,
                                    uint8_t v_Cr_sampling);

extern uint8_t ***init_MCU_3D(uint32_t nb_MCU, enum color_mode cm,
                              uint8_t sf_Y_h, uint8_t sf_Y_v);

extern uint8_t ****init_MCU_3D2(uint32_t nb_MCU, enum color_mode cm,
    uint8_t sf_Y_h, uint8_t sf_Y_v);

extern uint8_t ***init_result_3D(uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h,
                          uint8_t sf_Y_v);

extern uint8_t ***init_result_3D2(uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h,
                           uint8_t sf_Y_v, uint8_t h_Cb_sampling,
                           uint8_t h_Cr_sampling, uint8_t v_Cb_sampling,
                           uint8_t v_Cr_sampling);

extern void free_decoded_blocks_3D(int16_t ***decoded_blocks,
                                   uint32_t nb_blocks, enum color_mode cm,
                                   uint8_t sf_Y_h, uint8_t sf_Y_v);

extern void free_decoded_blocks_3D2(int16_t ***decoded_blocks, uint32_t nb_blocks,
                              enum color_mode cm, uint8_t sf_Y_h, uint8_t sf_Y_v,
                              uint8_t h_Cb_sampling, uint8_t h_Cr_sampling,
                              uint8_t v_Cb_sampling, uint8_t v_Cr_sampling);

extern void free_MCU_3D(uint8_t ***MCUs, uint32_t nb_blocks,
                        enum color_mode cm);

extern void free_result_3D(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                    uint8_t sf_Y_h, uint8_t sf_Y_v);

extern void free_result_3D2(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm,
                     uint8_t sf_Y_h, uint8_t sf_Y_v, uint8_t h_Cb_sampling,
                     uint8_t h_Cr_sampling, uint8_t v_Cb_sampling,
                     uint8_t v_Cr_sampling);

extern float *init_tab_cucl();

extern float **init_tab_idct();

extern void free_tab_idct(float **tab_idct_const);

#endif
