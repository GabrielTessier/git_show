
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
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

SDL_Surface** create_surfaces_from_text(TTF_Font* font, const char* text, SDL_Color color, int* nombre_surfaces) {
    *nombre_surfaces = 1;

    int text_size = strlen(text);
    char* copy_text = (char*) malloc(sizeof(char)*(text_size+1));
    strcpy(copy_text, text);
    for (int i=0; i<text_size; i++) {
        if (copy_text[i] == '\n') {
            copy_text[i] = 0;
            (*nombre_surfaces)++;
        }
    }

    SDL_Surface** s = malloc(sizeof(SDL_Surface*)**nombre_surfaces);
    char* ligne = copy_text;
    for (int i=0; i<*nombre_surfaces; i++) {
        s[i] = TTF_RenderUTF8_Solid(font, ligne, color);
        ligne = ligne + strlen(ligne) + 1;
    }

    free(copy_text);
    return s;
}

SDL_Rect* create_rect_from_text(TTF_Font* font, const char* text, int nombre_ligne, int x, int y) {
    int text_size = strlen(text);
    char* copy_text = (char*) malloc(sizeof(char)*(text_size+1));
    strcpy(copy_text, text);
    for (int i=0; i<text_size; i++) {
        if (copy_text[i] == '\n') copy_text[i] = 0;
    }
    SDL_Rect* rect = (SDL_Rect*) malloc(sizeof(SDL_Rect)*nombre_ligne);
    char* ligne = copy_text;
    for (int i=0; i<nombre_ligne; i++) {
        TTF_SizeText(font, ligne, &rect[i].w, &rect[i].h);
        rect[i].x = x;
        rect[i].y = y + 30*i;
        ligne = ligne + strlen(ligne) + 1;
    }
    free(copy_text);
    return rect;
}


SDL_bool in_rect(int x, int y, SDL_Rect r) {
  return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
}

void draw_text_at_mouse(SDL_Renderer *renderer, TTF_Font* font, char *text) {
  int x, y;
  SDL_GetMouseState(&x, &y);
  SDL_Color color = {0, 0, 0, 255};
  //  SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans
  int nb_surfaces = 0;
  SDL_Surface** surfaceMessage = create_surfaces_from_text(font, text, color, &nb_surfaces);
  SDL_Texture** textureMessage = (SDL_Texture**) malloc(sizeof(SDL_Texture*)*nb_surfaces);
  for (int i=0; i<nb_surfaces; i++) {
    textureMessage[i] = SDL_CreateTextureFromSurface(renderer, surfaceMessage[i]);
  }
  SDL_Rect *message_rect = create_rect_from_text(font, text, nb_surfaces, x + 10, y + 10);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for (int i=0; i<nb_surfaces; i++) {
    SDL_RenderCopy(renderer, textureMessage[i], NULL, &message_rect[i]);
  }
  for (int i = 0; i < nb_surfaces; i++) {
    SDL_FreeSurface(surfaceMessage[i]);
    SDL_DestroyTexture(textureMessage[i]);
  }
  free(surfaceMessage);
  free(textureMessage);
}
