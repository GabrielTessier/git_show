
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dict.h"

int val(char c) {
  if (c >= '0' && c <= '9') return c-'0';
  if (c >= 'a' && c <= 'f') return 10+c-'a';
  return -1;
}

dict_t* init_dict() {
  dict_t* dict = (dict_t*) malloc(sizeof(dict_t));
  for (int i=0; i<DICT_SIZE; i++) {
    dict->tab[i] = (array_t*) malloc(sizeof(array_t));
    dict->tab[i]->capa = 0;
    dict->tab[i]->size = 0;
    dict->tab[i]->data = NULL;
  }
  return dict;
}

int get_pos(char hash[41]) {
  int pos = 0;
  int pow = 1;
  for (int i=0; i<DICT_POW; i++) {
    pos += val(hash[i])*pow;
    pow *= 16;
  }
  return pos;
}

void add_dict(dict_t* d, char hash[41], type t, void* data) {
  int pos = get_pos(hash);

  array_t* arr = d->tab[pos];
  if (arr->size == arr->capa) {
    if (arr->capa == 0) arr->capa = 1;
    else arr->capa *= 2;
    arr->data = (elem_t**) realloc(arr->data, sizeof(elem_t*)*arr->capa);
  }
  arr->data[arr->size] = (elem_t*) malloc(sizeof(elem_t));
  strncpy(arr->data[arr->size]->hash, hash, 40);
  arr->data[arr->size]->hash[40] = 0;
  arr->data[arr->size]->type = t;
  arr->data[arr->size]->data = data;
  arr->size++;
}

elem_t* get_dict(dict_t* d, char hash[41]) {
  int pos = get_pos(hash);
  array_t* arr = d->tab[pos];
  for (int i=0; i<arr->size; i++) {
    if (strncmp(arr->data[i]->hash, hash, 40) == 0) return arr->data[i]->data;
  }
  return NULL;
}

void free_dict(dict_t* d, void free_elem (void*)) {
  for (int i=0; i<DICT_SIZE; i++) {
    for (int j=0; j<d->tab[i]->size; j++) {
      if (free_elem != NULL) free_elem(d->tab[i]->data[j]);
    }
    free(d->tab[i]->data);
    free(d->tab[i]);
  }
  free(d->tab);
  //free(d);
}
