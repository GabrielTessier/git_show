
#ifndef SDL_UTILS_H
#define SDL_UTILS_H


#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_stdinc.h>

struct key_list_s {
  int capa;
  int size;
  enum SDL_KeyCode *keys;
};
typedef struct key_list_s key_list_t;

struct keys_s {
  key_list_t* keys_down;
  key_list_t* keys_up;
  key_list_t* keys_pressed;
};
typedef struct keys_s keys_t;

key_list_t* init_key_list();
void add_key(key_list_t *key_list, enum SDL_KeyCode key);
void del_key(key_list_t *key_list, enum SDL_KeyCode key);
void free_key_list(key_list_t *key_list);
void free_keys(keys_t* k);
keys_t* init_keys();

SDL_bool poll_event (SDL_Event* event, keys_t* key_list);

#endif // SDL_UTILS_H
