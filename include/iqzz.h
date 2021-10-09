#ifndef __IQZZ_H__
#define __IQZZ_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void quantification_inverse(int16_t *decoded_block,
                                       uint8_t *quantification_table, int16_t *tab);

extern void zigzag_inverse(int16_t *tableau, int16_t *tab);


#endif
