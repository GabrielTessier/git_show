
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>

#include "sdl_utils.h"

key_list_t *init_key_list() {
  key_list_t *kl = (key_list_t *)malloc(sizeof(key_list_t));
  kl->capa = 1;
  kl->size = 0;
  kl->keys = (enum SDL_KeyCode*) malloc(sizeof(enum SDL_KeyCode));
  return kl;
}

void add_key(key_list_t *key_list, enum SDL_KeyCode key) {
  for (int i = 0; i < key_list->size; i++) {
    if (key_list->keys[i] == key) return;
  }
  if (key_list->size == key_list->capa) {
    key_list->capa *= 2;
    key_list->keys = (enum SDL_KeyCode *)realloc(key_list->keys, sizeof(enum SDL_KeyCode *) * key_list->capa);
  }
  key_list->keys[key_list->size] = key;
  key_list->size++;
}

void del_key(key_list_t *key_list, enum SDL_KeyCode key) {
  int d = 0;
  for (int i = 0; i < key_list->size; i++) {
    if (key_list->keys[i] == key) d++;
    if (i+d < key_list->size) key_list->keys[i] = key_list->keys[i+d];
  }
  key_list->size -= d;
  if (key_list->size <= key_list->capa/2 && key_list->capa >= 16) {
    key_list->capa /= 2;
    key_list->keys = (enum SDL_KeyCode *)realloc(key_list->keys, sizeof(enum SDL_KeyCode *) * key_list->capa);
  }
}

void free_key_list(key_list_t* key_list) {
  free(key_list->keys);
  free(key_list);
}

keys_t *init_keys() {
  keys_t *k = (keys_t *)malloc(sizeof(keys_t));
  k->keys_down = init_key_list();
  k->keys_pressed = init_key_list();
  k->keys_up = init_key_list();
  return k;
}

void free_keys(keys_t* k) {
  free_key_list(k->keys_down);
  free_key_list(k->keys_up);
  free_key_list(k->keys_pressed);
  free(k);
}


//void poll_event (SDL_Event* event, list_t** keyDown, SDL_bool* is_running_ptr, float* mouse_events) {
SDL_bool poll_event(SDL_Event *event, keys_t *key_list) {
  key_list->keys_up->size = 0;
  for (int i = 0; i < key_list->keys_down->size; i++) {
    add_key(key_list->keys_pressed, key_list->keys_down->keys[i]);
  }
  key_list->keys_down->size = 0;
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_KEYDOWN:
      add_key(key_list->keys_down, event->key.keysym.sym);
      break;
    case SDL_KEYUP:
      del_key(key_list->keys_pressed, event->key.keysym.sym);
      add_key(key_list->keys_up, event->key.keysym.sym);
      break;
    case SDL_MOUSEWHEEL:
      //mouse_events[0] = event->wheel.preciseY;
      break;
    case SDL_MOUSEMOTION:
      //mouse_events[1] = event->motion.xrel;
      //mouse_events[2] = event->motion.yrel;
      break;
    case SDL_QUIT:
      return SDL_TRUE;
    }
  }
  return SDL_FALSE;
}
