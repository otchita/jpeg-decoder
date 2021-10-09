
#include "bitstream.h"
#include "huffman.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct huff_table *arbre_init(void) {
  struct huff_table *arbre =
      (struct huff_table *)malloc(sizeof(struct huff_table));
  struct huff_table *arbrel =
      (struct huff_table *)malloc(sizeof(struct huff_table));
  struct huff_table *arbrer =
      (struct huff_table *)malloc(sizeof(struct huff_table));
  arbre->right = arbrer;
  arbre->left = arbrel;
  arbre->feuille = 0;
  return arbre;
}

void ajouter_valeur(struct huff_table *arbre, uint8_t niveau, uint32_t decalage,
                    int8_t element) {
  struct huff_table *courant = arbre;
  for (uint8_t i = 0; i < niveau; i++) {
    // on teste a chaque noeud si le decalage restant est supérieur ou inférieur
    // à 2^(niveau - i - 1) pour savoir si on va à gauche ou à droite.
    if (decalage >= (1 << (niveau - 1 - i))) {
      courant = (courant->right);
      // On ajuste le décalage si on va à droite
      decalage = decalage - (1 << (niveau - 1 - i));
    } else {
      courant = (courant->left);
    }
  }
  courant->feuille = 1;
  courant->right = NULL;
  courant->left = NULL;
  courant->elt = element;
}

void ajouter_noeud(struct huff_table *arbre, uint8_t niveau,
                   uint32_t decalage) {
  struct huff_table *courant = arbre;
  for (uint8_t i = 0; i < niveau; i++) {
    // on teste a chaque noeud si le decalage restant est supérieur ou inférieur
    // à 2^(niveau - i - 1) pour savoir  si on va à gauche ou à droite.
    if (decalage >= (1 << (niveau - 1 - i))) {
      courant = (courant->right);
      // On ajuste le décalage si on va à droite
      decalage = decalage - (1 << (niveau - 1 - i));
    } else {
      courant = (courant->left);
    }
  }
  struct huff_table *arbrel =
      (struct huff_table *)malloc(sizeof(struct huff_table));
  arbrel->right = NULL;
  arbrel->left = NULL;
  arbrel->feuille = 0;
  struct huff_table *arbrer =
      (struct huff_table *)malloc(sizeof(struct huff_table));
  arbrer->right = NULL;
  arbrer->left = NULL;
  arbrer->feuille = 0;

  courant->right = arbrer;
  courant->left = arbrel;
  courant->feuille = 0;
}

struct huff_table *load_huffman_table(struct bitstream *stream,
                                      uint16_t *nb_byte_read) {
  // On récupère les différents nombre d'éléments par niveau
  errno = 0;
  uint32_t tab[16];
  uint32_t byte = 0;
  *nb_byte_read = 0;
  for (uint8_t i = 0; i < 16; i++) {
    byte = 0;
    read_bitstream(stream, 8, &byte, true);
    tab[i] = byte;
    *nb_byte_read += 1;
  }
  // On crée l'arbre et on le remplit
  struct huff_table *arbre = arbre_init();

  uint32_t decalage = 0;
  uint32_t j = 0;
  uint32_t elt;
  for (uint8_t i = 0; i < 16; i++) {
    if (errno != 0) {
      *nb_byte_read = -1;
      return NULL;
    }
    // On remplit l'arbre
    // nombre de mots de longueur i+1 : tab[i]
    for (j = 0; j < tab[i]; j++) {
      read_bitstream(stream, 8, &elt, true);
      *nb_byte_read += 1;
      elt = (int8_t)elt;
      ajouter_valeur(arbre, i + 1, j + decalage, elt);
    }
    // on ajoute les noeuds restant
    for (uint32_t k = tab[i]; k < (1 << (i + 1)) - decalage; k++) {
      ajouter_noeud(arbre, i + 1, k + decalage + j - tab[i]);
    }
    decalage = (decalage + j) * 2;
  }
  return arbre;
}

int8_t next_huffman_value(struct huff_table *table, struct bitstream *stream) {
  struct huff_table *courant = table;
  uint32_t bit = 0;
  uint8_t compteur = 0;
  while (courant->feuille == 0) {
    if (compteur > 15) {
      fprintf(stderr, "[src/huffman.c] ERROR: Invalid huffman sequence\n");
      exit(1);
    }
    bit = 0;
    read_bitstream(stream, 1, &bit, true);
    if (bit == 0) {
      courant = courant->left;
    } else {
      courant = courant->right;
    }
    compteur += 1;
  }
  return (courant->elt);
}

// void free_huffman_table(struct huff_table *arbre){
//     if(arbre == NULL){
//       return;
//     }
//     free_huffman_table(arbre->left);
//     free_huffman_table(arbre->right);
//     free(arbre);
//     arbre = NULL;
// }

void free_huffman_table(struct huff_table *arbre) {
  if (arbre == NULL)
    return;
  free_huffman_table(arbre->left);
  free_huffman_table(arbre->right);
  free(arbre);
}
