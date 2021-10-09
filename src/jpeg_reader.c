#include "bitstream.h"
#include "huffman.h"
#include "jpeg_reader.h"

void read_COM_section(struct bitstream *stream, uint16_t *nb_byte_read) {
  uint32_t value;
  uint32_t length;
  read_bitstream(stream, 16, &length, false);
  nb_byte_read += length;
  read_bitstream(stream, (length - 2) * 8, &value, false);
}

void read_APP_section(struct bitstream *stream, uint16_t *nb_byte_read) {
  uint32_t value;
  read_bitstream(stream, 16, &value, false);
  nb_byte_read += 2;
  uint32_t offset = (value - 2) * 8;
  for (size_t i = 0; i < (offset / 8); i++) {
    read_bitstream(stream, 8, &value, false);
    nb_byte_read += 1;
  }
}

void read_DQT_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read) {
  uint32_t value;
  uint32_t length;
  uint32_t index;
  read_bitstream(jdesc->stream, 16, &length, false);
  nb_byte_read += length;
  read_bitstream(jdesc->stream, 4, &value, false);
  read_bitstream(jdesc->stream, 4, &index, false);
  jdesc->nb_q_tables += (uint8_t)((length - 2) / 65);
  uint8_t *q_table = (uint8_t *)malloc(64 * sizeof(uint8_t *));
  for (size_t j = 0; j < 64; j++) {
    read_bitstream(jdesc->stream, 8, &value, false);
    q_table[j] = (uint8_t)value;
  }
  jdesc->q_tables[index] = q_table;
}

void read_SOF_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read) {
  uint32_t value;
  uint32_t length;
  uint16_t rest;
  read_bitstream(jdesc->stream, 16, &length, false);
  nb_byte_read += length;
  read_bitstream(jdesc->stream, 8, &value, false);
  read_bitstream(jdesc->stream, 16, &value, false);
  jdesc->height = value;
  read_bitstream(jdesc->stream, 16, &value, false);
  jdesc->width = value;
  read_bitstream(jdesc->stream, 8, &value, false);
  jdesc->nb_components = (uint8_t)value;
  rest = (length - 8) / 3;
  uint8_t *frame_comp_ids =
      (uint8_t *)malloc(jdesc->nb_components * sizeof(uint8_t));
  uint8_t *frame_comp_sf =
      (uint8_t *)malloc(jdesc->nb_components * 2 * sizeof(uint8_t));
  uint8_t *frame_comp_q_indexes =
      (uint8_t *)malloc(jdesc->nb_components * sizeof(uint8_t));
  for (size_t i = 0; i < rest; i++) {
    read_bitstream(jdesc->stream, 8, &value, false);
    frame_comp_ids[i] = (uint8_t)value;
    read_bitstream(jdesc->stream, 4, &value, false);
    frame_comp_sf[2 * i] = (uint8_t)value;
    read_bitstream(jdesc->stream, 4, &value, false);
    frame_comp_sf[2 * i + 1] = (uint8_t)value;
    read_bitstream(jdesc->stream, 8, &value, false);
    frame_comp_q_indexes[i] = (uint8_t)value;
  }
  jdesc->frame_comp_sf = frame_comp_sf;
  jdesc->frame_comp_ids = frame_comp_ids;
  jdesc->frame_comp_q_indexes = frame_comp_q_indexes;
}

void read_DHT_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read) {
  uint32_t value;
  uint32_t length;
  uint16_t rest;
  uint32_t index;
  read_bitstream(jdesc->stream, 16, &length, false);
  rest = (length - 2) / 3;
  read_bitstream(jdesc->stream, 3, &value, false);
  if (value != 0) {
    fprintf(stderr, "[src/jpeg_reader.c] ERROR:  the first three bits in the "
                    "0x04 DHT offset are not null\n");
    exit(1);
  }
  read_bitstream(jdesc->stream, 1, &value, false);
  if (value == 0) {
    jdesc->nb_h_tables_dc += 1;
  } else {
    jdesc->nb_h_tables_ac += 1;
  }
  read_bitstream(jdesc->stream, 4, &index, false);
  nb_byte_read += 3;
  struct huff_table *h_table = load_huffman_table(jdesc->stream, nb_byte_read);
  if (nb_byte_read >= 0) {
    jdesc->h_tables[2 * value + index] = h_table;
  } else {
    exit(1);
  }
}

void read_SOS_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read) {
  uint32_t value;
  uint32_t length;
  uint8_t nb_components;
  read_bitstream(jdesc->stream, 16, &length, false);
  nb_byte_read += length;
  read_bitstream(jdesc->stream, 8, &value, false);
  nb_components = (uint8_t)value;
  uint8_t *scan_comp_ids = (uint8_t *)malloc(nb_components * sizeof(uint8_t));
  uint8_t *scan_comp_h_indexes =
      (uint8_t *)malloc(nb_components * 2 * sizeof(uint8_t));
  for (size_t i = 0; i < nb_components; i++) {
    read_bitstream(jdesc->stream, 8, &value, false);
    scan_comp_ids[i] = (uint8_t)value;
    read_bitstream(jdesc->stream, 4, &value, false);
    scan_comp_h_indexes[2 * i] = (uint8_t)value;
    read_bitstream(jdesc->stream, 4, &value, false);
    scan_comp_h_indexes[2 * i + 1] = (uint8_t)value;
  }
  read_bitstream(jdesc->stream, 8, &value, false);
  read_bitstream(jdesc->stream, 8, &value, false);
  read_bitstream(jdesc->stream, 8, &value, false);
  jdesc->scan_comp_ids = scan_comp_ids;
  jdesc->scan_comp_h_indexes = scan_comp_h_indexes;
}

void read_sections(struct jpeg_desc *jdesc, uint16_t *nb_byte_read,
                   uint32_t entry_value) {
  switch (entry_value) {
  case 0xffe0:
    /* application Section */
    read_APP_section(jdesc->stream, nb_byte_read);
    break;
  case 0xfffe:
    /* Commentary Section */
    read_COM_section(jdesc->stream, nb_byte_read);
    break;
  case 0xffdb:
    /* The Define Quantization Table Section */
    read_DQT_section(jdesc, nb_byte_read);

    break;
  case 0xffc0:
    /* The Start Of Frame Section */
    read_SOF_section(jdesc, nb_byte_read);
    break;
  case 0xffc4:
    /* The Define Huffman Table Section */
    read_DHT_section(jdesc, nb_byte_read);
    break;
  case 0xffda:
    /* The Start Of Scan Section */
    read_SOS_section(jdesc, nb_byte_read);
    break;
  default:
    if (entry_value / 16 == 0xffc) {
      fprintf(stderr, "[src/jpeg_reader.c] ERROR:  the section of frame mode "
                      "SOF%u is not handled by this decoder (SOF0 only), "
                      "meaning JPEG Baseline DCT (Huffman) only\n",
              entry_value % 16);
    } else if (entry_value / 16 == 0xffe) {
      fprintf(stderr, "[src/jpeg_reader.c] ERROR:  application APP mode APP%u "
                      "is not handled by this decoder (APP0 only)\n",
              entry_value % 16);
    } else if (entry_value / 256 != 0xff) {
      fprintf(stderr, "[src/jpeg_reader.c] ERROR: Invalid marker, expected "
                      "0xff, got 0x%x\n",
              entry_value % 256);
    } else {
      fprintf(stderr, "[src/jpeg_reader.c] ERROR:  something went wrong while "
                      "reading the bitstream!\n");
    }
    exit(1);
  }
}

bool is_the_file_jpeg(const char *filename) {
  uint8_t i = 0;
  do {
    i++;
  } while (filename[i] != '.' && i < (strlen(filename) - 1));
  if (i == (strlen(filename) - 1)) {
    return false;
  } else if (filename[i + 1] == 'j' && filename[i + 2] == 'p') {
    if (filename[i + 3] == 'g' && (i + 4) == strlen(filename)) {
      return true;
    } else if (filename[i + 3] == 'e' && filename[i + 4] == 'g' &&
               (i + 5) == strlen(filename)) {
      return true;
    }
  } else if (filename[i + 1] == 'J' && filename[i + 2] == 'P' &&
             filename[i + 3] == 'G' && (i + 4) == strlen(filename)) {
    return true;
  }
  return false;
}

// general
struct jpeg_desc *read_jpeg(const char *filename) {
  uint32_t value;
  uint16_t nb_byte_read = 0;
  struct jpeg_desc *jdesc = (struct jpeg_desc *)malloc(sizeof(*jdesc));
  jdesc->filename = filename;
  FILE *file = fopen(filename, "rb");
  struct bitstream *stream = create_bitstream(filename);
  jdesc->stream = stream;
  if (!is_the_file_jpeg(jdesc->filename)) {
    fprintf(stderr,
            "Invalid file extension, only .jpg, .jpeg, or .JPG are expected\n");
    exit(1);
  }
  if (stream == NULL) {
    fprintf(
        stderr,
        "[src/jpeg_reader.c] ERROR: unable to create bitstream from file %s\n",
        get_filename(jdesc));
    exit(1);
  }
  struct huff_table **h_tables =
      (struct huff_table **)malloc(4 * sizeof(struct huff_table *));
  for (size_t i = 0; i < 4; i++) {
    h_tables[i] = NULL;
  }
  jdesc->h_tables = h_tables;
  uint8_t **q_tables = (uint8_t **)malloc(2 * sizeof(*q_tables));
  for (size_t i = 0; i < 2; i++) {
    q_tables[i] = NULL;
  }
  jdesc->q_tables = q_tables;
  read_bitstream(jdesc->stream, 16, &value, false);
  nb_byte_read += 2;
  if (value != 0xffd8) {
    fprintf(
        stderr,
        "[src/jpeg_reader.c] ERROR: Invalid marker, expected 0xff, got 0x%x\n",
        value % 256);
    exit(1);
  }
  do {
    read_bitstream(jdesc->stream, 16, &value, false);
    nb_byte_read += 2;
    read_sections(jdesc, &nb_byte_read, value);
  } while (value != 0xffda);
  fclose(file);
  return jdesc;
}

void close_jpeg(struct jpeg_desc *jdesc) {
  close_bitstream(jdesc->stream);

  for (size_t i = 0; i < 2; i++) {
    free(jdesc->q_tables[i]);
  }
  free(jdesc->q_tables);

  free(jdesc->frame_comp_sf);
  free(jdesc->frame_comp_ids);
  free(jdesc->frame_comp_q_indexes);

  for (size_t i = 0; i < 4; i++) {
    free_huffman_table(jdesc->h_tables[i]);
  }
  free(jdesc->h_tables);

  free(jdesc->scan_comp_ids);
  free(jdesc->scan_comp_h_indexes);
  free(jdesc);
}

char *get_filename(const struct jpeg_desc *jdesc) {
  return (char *)jdesc->filename;
}

// access to stream, placed just at the beginning of the scan raw data
struct bitstream *get_bitstream(const struct jpeg_desc *jdesc) {
  return jdesc->stream;
}

// from DQT
uint8_t get_nb_quantization_tables(const struct jpeg_desc *jdesc) {
  return jdesc->nb_q_tables;
}
uint8_t *get_quantization_table(const struct jpeg_desc *jdesc, uint8_t index) {
  return jdesc->q_tables[index];
}

// from DHT
uint8_t get_nb_huffman_tables(const struct jpeg_desc *jdesc, enum acdc acdc) {
  switch (acdc) {
  case DC:
    return jdesc->nb_h_tables_dc;
    break;
  case AC:
    return jdesc->nb_h_tables_ac;
    break;
  default:
    fprintf(stderr,
            "[src/jpeg_reader.c] ERROR:  Huffman table index out of range\n");
    exit(1);
  }
}

struct huff_table *get_huffman_table(const struct jpeg_desc *jdesc,
                                     enum acdc acdc, uint8_t index) {
  if ((acdc + index) > 2) {
    fprintf(stderr,
            "[src/jpeg_reader.c] ERROR:  Huffman table index out of range\n");
    exit(1);
  } else if (jdesc->h_tables[2 * acdc + index] != NULL) {
    return jdesc->h_tables[2 * acdc + index];
  } else {
    fprintf(stderr, "[src/jpeg_reader.c] ERROR:  Invalid Huffman sequence\n");
    exit(1);
  }
}

// from Frame Header SOF0
uint16_t get_image_size(struct jpeg_desc *jdesc, enum direction dir) {
  switch (dir) {
  case DIR_H:
    return jdesc->width;
    break;
  case DIR_V:
    return jdesc->height;
    break;
  default:
    fprintf(stderr, "[src/jpeg_reader.c] ERROR:  the direction in "
                    "get_image_size is out of place\n");
    exit(1);
  }
}

uint8_t get_nb_components(const struct jpeg_desc *jdesc) {
  return jdesc->nb_components;
}

uint8_t get_frame_component_id(const struct jpeg_desc *jdesc,
                               uint8_t frame_comp_index) {
  return jdesc->frame_comp_ids[frame_comp_index];
}
uint8_t get_frame_component_sampling_factor(const struct jpeg_desc *jdesc,
                                            enum direction dir,
                                            uint8_t frame_comp_index) {
  return jdesc->frame_comp_sf[2 * frame_comp_index + dir];
}
uint8_t get_frame_component_quant_index(const struct jpeg_desc *jdesc,
                                        uint8_t frame_comp_index) {
  return jdesc->frame_comp_q_indexes[frame_comp_index];
}

// from Scan Header SOS
uint8_t get_scan_component_id(const struct jpeg_desc *jdesc,
                              uint8_t scan_comp_index) {
  return jdesc->scan_comp_ids[scan_comp_index];
}
uint8_t get_scan_component_huffman_index(const struct jpeg_desc *jdesc,
                                         enum acdc acdc,
                                         uint8_t scan_comp_index) {
  return jdesc->scan_comp_h_indexes[2 * scan_comp_index + acdc];
}
