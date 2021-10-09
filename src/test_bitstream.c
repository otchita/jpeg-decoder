#include "bitstream.h"
#include <math.h>

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    fprintf(stdout, "Usage: %s fichier.jpeg\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *filename = argv[1];
  struct bitstream *stream = create_bitstream(filename);
  // fprintf(stdout, "%ld\n", ftell(stream->file));
  uint32_t nombre, i = 0;
  // int k = 0;
  // int j;
  // uint32_t p;
  while (1) {
    if (end_of_bitstream(stream)) {
      fprintf(stdout, "FIN\n");
      break;
    }
    // read_bitstream(stream, 32, &nombre, true);

    // j = read_bitstream(stream, 0, &nombre, true);
    // fprintf(stderr, "%d %u\n", j, nombre);
    /* affichage nb bits variable */
    // j = i % 33;
    // read_bitstream(stream, j, &nombre, true);
    // for (int b = j; b > 0; b--, k++) {
    //   p = (uint32_t)(nombre / pow(2, b - 1));
    //   fprintf(stdout, "%u", p);
    //   nombre -= p * pow(2, b - 1);
    //   if (k % 32 == 31) {
    //     fprintf(stdout, "\n");
    //   }
    //   if (k % 128 == 127) {
    //     fprintf(stdout, "\n");
    //   } else if (k % 64 == 63) {
    //     fprintf(stdout, "\n");
    //   } else if (k % 8 == 7) {
    //     fprintf(stdout, " ");
    //   }
    // }

    /* affichage octet par octet */
    read_bitstream(stream, 32, &nombre, true);
    if (i % 16 == 0) {
      fprintf(stdout, "0000%x  ", i);
    }
    fprintf(stdout, "%x%x ", nombre / 16, nombre % 16);
    if (i % 16 == 15) {
      fprintf(stdout, "\n");
    } else if (i % 8 == 7){
      fprintf(stdout, "  ");
    }

    /* affichage bit par bit */
    // read_bitstream(stream, 1, &nombre, false);
    // if (i % 128 == 0) {
    //   fprintf(stdout, "0000%x  ", i / 8);
    // }
    // fprintf(stdout, "%u", nombre);
    // if (i % 128 == 127) {
    //   fprintf(stdout, "\n");
    // } else if (i % 64 == 63) {
    //   fprintf(stdout, "\t\t");
    // } else if (i % 8 == 7) {
    //   fprintf(stdout, " ");
    // }
    i++;
    // fprintf(stdout, "i = %u\n", i);
    // }
  }
  fprintf(stdout, "\n");
  close_bitstream(stream);
  return EXIT_SUCCESS;
}
