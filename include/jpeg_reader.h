#ifndef __JPEG_DESC_H__
#define __JPEG_DESC_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "bitstream.h"


enum component {
    COMP_Y,
    COMP_Cb,
    COMP_Cr,
    /* sentinelle */
    COMP_NB
};

enum direction {
    DIR_H,
    DIR_V,
    /* sentinelle */
    DIR_NB
};

enum acdc {
    DC = 0,
    AC = 1,
    /* sentinelle */
    ACDC_NB
};

struct jpeg_desc {
  const char *filename;
  struct bitstream *stream;

  uint8_t nb_q_tables;
  uint8_t **q_tables;

  uint16_t height;
  uint16_t width;
  uint8_t nb_components;

  uint8_t *frame_comp_ids;
  uint8_t *frame_comp_sf;
  uint8_t *frame_comp_q_indexes;

  uint8_t nb_h_tables_dc;
  uint8_t nb_h_tables_ac;
  struct huff_table **h_tables;

  uint8_t *scan_comp_ids;
  uint8_t *scan_comp_h_indexes;
};


// general
extern struct jpeg_desc *read_jpeg(const char *filename);
extern void close_jpeg(struct jpeg_desc *jpeg);
extern char *get_filename(const struct jpeg_desc *jpeg);

extern void read_sections(struct jpeg_desc *jdesc,
                          uint16_t *nb_byte_read, uint32_t entry_value);

extern bool is_the_file_jpeg(const char *filename);
extern void read_APP_section(struct bitstream *stream, uint16_t *nb_byte_read);
extern void read_COM_section(struct bitstream *stream, uint16_t *nb_byte_read);
extern void read_DQT_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read);
extern void read_SOF_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read);
extern void read_DHT_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read);
extern void read_SOS_section(struct jpeg_desc *jdesc, uint16_t *nb_byte_read);

// access to stream, placed just at the beginning of the scan raw data
extern struct bitstream *get_bitstream(const struct jpeg_desc *jpeg);

// from DQT
extern uint8_t get_nb_quantization_tables(const struct jpeg_desc *jpeg);
extern uint8_t *get_quantization_table(const struct jpeg_desc *jpeg,
                                       uint8_t index);

// from DHT
extern uint8_t get_nb_huffman_tables(const struct jpeg_desc *jpeg,
                                     enum acdc acdc);
extern struct huff_table *get_huffman_table(const struct jpeg_desc *jpeg,
                                            enum acdc acdc, uint8_t index);

// from Frame Header SOF0
extern uint16_t get_image_size(struct jpeg_desc *jpeg, enum direction dir);
extern uint8_t get_nb_components(const struct jpeg_desc *jpeg);

extern uint8_t get_frame_component_id(const struct jpeg_desc *jpeg,
                                      uint8_t frame_comp_index);
extern uint8_t get_frame_component_sampling_factor(const struct jpeg_desc *jpeg,
                                                   enum direction dir,
                                                   uint8_t frame_comp_index);
extern uint8_t get_frame_component_quant_index(const struct jpeg_desc *jpeg,
                                               uint8_t frame_comp_index);

// from Scan Header SOS
extern uint8_t get_scan_component_id(const struct jpeg_desc *jpeg,
                                     uint8_t scan_comp_index);
extern uint8_t get_scan_component_huffman_index(const struct jpeg_desc *jpeg,
                                                enum acdc,
                                                uint8_t scan_comp_index);

#endif
