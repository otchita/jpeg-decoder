#include "bitstream.h"

struct bitstream *create_bitstream(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    return NULL;
  }
  struct bitstream *stream = malloc(sizeof(*stream));
  stream->file = file;
  stream->position = (uint32_t *)malloc(3 * sizeof(uint32_t));
  for (size_t i = 0; i < 3; i++) {
    stream->position[i] = 0;
  }
  stream->last_byte = (uint8_t *)malloc(sizeof(uint8_t));
  *(stream->last_byte) = 0;
  return stream;
}

void close_bitstream(struct bitstream *stream) {
  free(stream->position);
  free(stream->last_byte);
  fclose(stream->file);
  free(stream);
}

uint8_t read_bitstream(struct bitstream *stream, uint8_t nb_bits,
                       uint32_t *dest, bool discard_byte_stuffing) {

  if (nb_bits > 32) {
    fprintf(stdout, "The number of bits to read is too big !\n");
    return EXIT_FAILURE;
  }
  uint8_t c;
  *dest = 0;
  uint32_t nb_bits_read = 0;
  // back to the byte being read
  uint32_t count = 0;
  while (nb_bits_read < nb_bits) {
    fseek(stream->file, 1024 * stream->position[2] + stream->position[1],
          SEEK_SET);
    if (end_of_bitstream(stream)) {
      break;
    }
    c = (uint8_t)fgetc(stream->file);

    if (discard_byte_stuffing && *(stream->last_byte) == 255 && c == 0) {
      *(stream->last_byte) = 0;
      stream->position[1]++;
      if (stream->position[1] == 1024) {
        stream->position[1] = 0;
        stream->position[2]++;
      }
      continue;
    }
    uint8_t j = (uint8_t)stream->position[0];
    while (j < 8 && nb_bits_read < nb_bits) {
      *dest = *dest << 1;
      *dest += (c >> (7 - j)) & 00000001;
      nb_bits_read++;
      j++;
      stream->position[0] = j % 8;
      if (stream->position[0] == 0) {
        stream->position[1]++;
        *(stream->last_byte) = c;
      }
      if (stream->position[1] == 1024) {
        stream->position[1] = 0;
        stream->position[2]++;
      }
    }
    count++;
  };
  if (nb_bits_read != nb_bits) {
    fprintf(stderr, "Bitstream error : didn't read the right number of bits ! "
                    "(%u instead of %u)\n",
            nb_bits_read, nb_bits);
  }
  return nb_bits_read;
}

bool end_of_bitstream(struct bitstream *stream) {
  char temp;
  fseek(stream->file, 1024 * stream->position[2] + stream->position[1],
        SEEK_SET);
  uint8_t n = fread(&temp, 1, 1, stream->file);
  fseek(stream->file, 1024 * stream->position[2] + stream->position[1],
        SEEK_SET);
  if (n == 0) {
    return true;
  } else {
    return false;
  }
}
