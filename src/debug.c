#include "debug.h"
#include "jpeg_reader.h"

void print_decoded_dcac_block(int16_t *block, size_t index) {
  uint8_t i;
  fprintf(stdout, "[%zu\t", index);
  for (i = 0; i < 64; i++) {
    fprintf(stdout, "%hx ", block[i]);
  }
  fprintf(stdout, "\t]\n\n");
}

void print_result(uint8_t *block, uint32_t index) {
  uint8_t i;
  fprintf(stdout, "[%u\t", index);
  for (i = 0; i < 64; i++) {
    fprintf(stdout, "%x ", block[i]);
  }
  fprintf(stdout, "\t]\n\n");
}

void print_post_qt(int16_t *block, uint32_t index) {
  uint8_t i;
  fprintf(stdout, "[%u\t", index);
  for (i = 0; i < 64; i++) {
    fprintf(stdout, "%hx ", block[i]);
  }
  fprintf(stdout, "\t]\n\n");
}
void print_idct(uint8_t *tab_apres_idct) {
  printf("[  idct] ");
  for (uint8_t i = 0; i < 64; i++) {
    printf(" %x", tab_apres_idct[i]);
  }
  printf("\n");
}

void print_zz(int16_t *tab_apres_zz) {
  printf("[   izz] ");
  for (uint8_t i = 0; i < 64; i++) {
    printf(" %hx", tab_apres_zz[i]);
  }
  printf("\n\n");
}

void print_iq(int16_t *tab_apres_quantif) {
  printf("[iquant] ");
  for (uint8_t i = 0; i < 64; i++) {
    printf(" %hx", tab_apres_quantif[i]);
  }
  printf("\n\n");
}

void print_jdesc(struct jpeg_desc *jdesc) {
  fprintf(stdout, "nb_q_tables = %u\n", jdesc->nb_q_tables);
  fprintf(stdout, "taille_h x taille_v is (%u x %u)\n", jdesc->width,
          jdesc->height);
  fprintf(stdout, "nb_components = %u\n", jdesc->nb_components);
  uint8_t cm = get_nb_components(jdesc);
  for (size_t i = 0; i < cm; i++) {
    fprintf(stdout, "frame_comp_ids[%zu] %u\n", i, jdesc->frame_comp_ids[i]);
    fprintf(stdout, "frame_comp_sf %zu (h x v) = (%u x %u)\n", i,
            jdesc->frame_comp_sf[2 * i], jdesc->frame_comp_sf[2 * i + 1]);
    fprintf(stdout, "frame_comp_q_indexes[%zu] %u\n", i,
            jdesc->frame_comp_q_indexes[i]);
  }
  fprintf(stdout, "nb_h_tables_dc x nb_h_tables_ac = %u x %u\n",
          jdesc->nb_h_tables_dc, jdesc->nb_h_tables_ac);
  for (size_t i = 0; i < cm; i++) {
    fprintf(stdout, "scan_comp_ids[%zu] %u\n", i, jdesc->scan_comp_ids[i]);
    fprintf(stdout, "scan_comp_h_indexes %zu (h x v) = (%u x %u)\n", i,
            jdesc->scan_comp_h_indexes[2 * i],
            jdesc->scan_comp_h_indexes[2 * i + 1]);
  }
}
