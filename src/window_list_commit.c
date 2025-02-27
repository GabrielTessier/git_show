

#include "window_list_commit.h"
#include "dict.h"
#include "sdl_utils.h"
#include "view.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>

struct vue_s {
  int capa;
  int size;
  commit_t **vue;
  int *x;
  int *y;
};
typedef struct vue_s vue_t;

void add_vue(vue_t* vue, commit_t *c) {
  vue->size++;
  if (vue->size == vue->capa) {
    vue->capa *= 2;
    vue->vue = (commit_t **)realloc(vue->vue, sizeof(commit_t *) * vue->capa);
    vue->x = (int *)realloc(vue->x, sizeof(int) * vue->capa);
    vue->y = (int *)realloc(vue->y, sizeof(int) * vue->capa);
  }
  vue->vue[vue->size] = c;
}

int draw_list_commit(SDL_Renderer* renderer, dict_t* all_hashes, commit_t* c, int size, int x, int y, int* prx, int* pry, commit_t** over, vue_t* vue) {
  for (int i = 0; i < vue->size; i++) {
    *prx = vue->x[i];
    *pry = vue->y[i];
    if (vue->vue[i] == c) return 0;
  }
  int* childx = (int*) malloc(sizeof(int)*c->nb_parent);
  int* childy = (int*) malloc(sizeof(int)*c->nb_parent);
  int cx = x;
  add_vue(vue, c);
  int indice_in_vue = vue->size;
  for (int i = 0; i < c->nb_parent; i++) {
    commit_t* p = (commit_t*) get_dict(all_hashes, c->parents[i]);
    cx += draw_list_commit(renderer, all_hashes, p, size, cx, y + size * 2,
                           childx + i, childy + i, over, vue);
    if (i != c->nb_parent - 1) cx += size;
  }
  if (c->nb_parent == 0) cx += size;

  SDL_Rect test_rect;
  test_rect.x = (int) ((cx-x)/2 + x) - size/2;
  test_rect.y = y;
  test_rect.w = size;
  test_rect.h = size;
  SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
  SDL_RenderFillRect(renderer, &test_rect);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for (int i=0; i<c->nb_parent; i++) {
    SDL_RenderDrawLine(renderer, test_rect.x+size/2, test_rect.y+size/2, childx[i]+size/2, childy[i]+size/2);
  }
  free(childx);
  free(childy);
  vue->x[indice_in_vue] = test_rect.x;
  vue->y[indice_in_vue] = test_rect.y;
  if (prx != NULL) *prx = test_rect.x;
  if (pry != NULL) *pry = test_rect.y;

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  if (in_rect(mouseX, mouseY, test_rect)) {
    *over = c;
  }

  return cx-x;
}

int window_list_commit(int *window_to_draw, SDL_Renderer *renderer, dict_t *all_hashes, commit_t *c, TTF_Font *font, commit_t** commit_out) {
  int x = 100;
  int y = 100;
  int size = 50;
  SDL_bool is_running = SDL_TRUE;
  SDL_Event event;
  keys_t *keys = init_keys();
  while (is_running) {
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);

    commit_t *over = NULL;
    vue_t vue;
    vue.capa = 1;
    vue.size = 0;
    vue.vue = (commit_t**) malloc(sizeof(commit_t*));
    vue.x = (int*) malloc(sizeof(int*));
    vue.y = (int*) malloc(sizeof(int*));
    draw_list_commit(renderer, all_hashes, c, size, x, y, NULL, NULL, &over, &vue);
    free(vue.vue);
    free(vue.x);
    free(vue.y);
    if (over != NULL) {
      draw_text_at_mouse(renderer, font, over->hash);
      *commit_out = over;
    }

    is_running = !poll_event(&event, keys);

    for (int i = 0; i < keys->keys_down->size; i++) {
      switch (keys->keys_down->keys[i]) {
      case SDLK_LEFT:
        x += 100;
        break;
      case SDLK_RIGHT :
        x -= 100;
        break;
      case SDLK_UP:
        y += 100;
        break;
      case SDLK_DOWN:
        y -= 100;
        break;
      case SDLK_e:
        size += 10;
        break;
      case SDLK_z:
        size -= 10;
        if (size <= 0) size = 1;
        break;
      case SDLK_ESCAPE:
        *window_to_draw = 0;
        is_running = SDL_FALSE;
        break;
      case SDLK_i:
        *window_to_draw = 2;
        is_running = SDL_FALSE;
      default:
        break;
      }
    }

    SDL_RenderPresent(renderer);
  }
  free_keys(keys);

  return 0;
}
