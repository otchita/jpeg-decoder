#ifndef __TEST_BITSTREAM_H__
#define __TEST_BITSTREAM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


struct bitstream {
  /* the file from which we are reading */
  FILE *file;
  /* the bit number currently being read
  coded on 3 numbers of uint32_t :
  [n_bits, n_bytes, n_kbytes] */
  uint32_t *position;
  /*  the last byte we read */
  char last_byte;
  // uint32_t SOI;
  // uint32_t APP0;
  // uint32_t DQT;
  // uint32_t SOF0;
  // uint32_t DHT;
  // uint32_t SOS;
  // uint32_t EOI;
};

extern struct bitstream *create_bitstream(const char *filename);

extern void close_bitstream(struct bitstream *stream);

extern uint8_t read_bitstream(struct bitstream *stream,
                              uint8_t nb_bits,
                              uint32_t *dest,
                              bool discard_byte_stuffing);

extern bool end_of_bitstream(struct bitstream *stream);

/* Optionnel! */
// extern void skip_bitstream_until(struct bitstream *stream, uint8_t byte);



#endif
