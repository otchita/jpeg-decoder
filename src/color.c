#include "color.h"
#include "idct.h"
#include "main.h"

// converts YCbCr to RGB
uint8_t *conversionRGB(uint8_t Y, uint8_t Cb, uint8_t Cr) {
  float R;
  float G;
  float B;
  uint8_t *RGB = (uint8_t *)malloc(3 * sizeof(uint8_t));
  Y = (float)Y;
  Cb = (float)Cb;
  Cr = (float)Cr;
  R = Y - 0.0009267 * (Cb - 128) + 1.4016868 * (Cr - 128);
  G = Y - 0.3436954 * (Cb - 128) - 0.7141690 * (Cr - 128);
  B = Y + 1.7721604 * (Cb - 128) + 0.0009902 * (Cr - 128);
  clampage(&R);
  clampage(&G);
  clampage(&B);
  R = (uint8_t)R;
  G = (uint8_t)G;
  B = (uint8_t)B;
  RGB[0] = R;
  RGB[1] = G;
  RGB[2] = B;
  return RGB;
}

/*
returns the MCUs with colors encoded in RGB
*/
uint8_t ***RGB(uint8_t ***YCbCr_MCUs, uint32_t nb_MCU, uint8_t sf_Y_h,
               uint8_t sf_Y_v) {
  uint8_t ***RGB_MCUs = init_MCU_3D(nb_MCU, 3, sf_Y_h, sf_Y_v);
  for (size_t i = 0; i < nb_MCU; i++) {
    for (size_t j = 0; j < 64 * sf_Y_h * sf_Y_v; j++) {
      uint8_t *RGB = conversionRGB(YCbCr_MCUs[i][0][j], YCbCr_MCUs[i][1][j],
                                   YCbCr_MCUs[i][2][j]);
      RGB_MCUs[i][0][j] = RGB[0];
      RGB_MCUs[i][1][j] = RGB[1];
      RGB_MCUs[i][2][j] = RGB[2];
      free(RGB);
    }
  }
  return RGB_MCUs;
}
