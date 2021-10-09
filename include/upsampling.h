#ifndef __UPSAMPLING_H__
#define __UPSAMPLING_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "structures.h"
#include "main.h"

extern uint8_t *join_v(uint8_t *liste1, uint8_t *liste2);

extern uint8_t *join_v2(uint8_t **v_blocks, uint8_t height);

extern uint8_t *join_h(uint8_t *liste1, uint8_t *liste2);

extern uint8_t *join_h2(uint8_t **h_blocks, uint8_t length);

extern uint8_t *join_hv(uint8_t *block1, uint8_t *block2, uint8_t *block3,
                 uint8_t *block4);

extern uint8_t *join_hv2(uint8_t **blocks, uint8_t length, uint8_t height);

extern uint8_t **upsample_MCU_h(uint8_t *h_mcu);

extern uint8_t **upsample_MCU_h2(uint8_t *h_mcu, uint8_t length);

extern uint8_t **upsample_MCU_v(uint8_t *v_mcu);

extern uint8_t **upsample_MCU_v2(uint8_t *v_mcu, uint8_t height);

extern uint8_t **upsample_MCU_hv(uint8_t *hv_mcu);

extern uint8_t **upsample_MCU_hv2(uint8_t *hv_mcu, uint8_t length, uint8_t height);

extern uint8_t ***upsample(uint8_t ***result, uint32_t nb_blocks, enum color_mode cm,
                      uint8_t sf_Y_h, uint8_t sf_Y_v);

extern uint8_t ***echanti2(uint8_t ***result, uint32_t nb_blocks, enum color_mode cm,
                      uint8_t sf_Y_h, uint8_t sf_Y_v, uint32_t nb_blocks_h, uint32_t nb_blocks_v,
                    uint8_t h_Cb_sampling, uint8_t h_Cr_sampling,
                  uint8_t v_Cb_sampling, uint8_t v_Cr_sampling);

extern uint8_t ***upsample2(uint8_t ***result, uint32_t nb_MCU, enum color_mode cm, uint8_t sf_Y_h, uint8_t sf_Y_v,
                    uint8_t h_Cb_sampling, uint8_t h_Cr_sampling, uint8_t v_Cb_sampling,
                    uint8_t v_Cr_sampling);

#endif
