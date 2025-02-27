
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "view.h"
#include "sdl_utils.h"

int draw_blob(SDL_Renderer* renderer, blob_t* b, int size, int x, int y, int* prx, int* pry, char** text) {
//  printf("%s (%s)\n", b->name, b->hash);
  SDL_Rect test_rect;
  test_rect.x = x;
  test_rect.y = y;
  test_rect.w = size;
  test_rect.h = size;
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_RenderFillRect(renderer, &test_rect);

  if (prx != NULL) *prx = test_rect.x;
  if (pry != NULL) *pry = test_rect.y;

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  if (in_rect(mouseX, mouseY, test_rect)) {
    *text = b->name;
  }

  return size;
}

int draw_tree(SDL_Renderer* renderer, dict_t* all_hashes, tree_t* t, int size, int x, int y, int* prx, int* pry, char** text) {
//  printf("%s (%s)\n", t->name, t->hash);

  int* childx = (int*) malloc(sizeof(int)*(t->nb_blob+t->nb_tree));
  int* childy = (int*) malloc(sizeof(int)*(t->nb_blob+t->nb_tree));
  int cx = x;
  for (int i=0; i<t->nb_blob; i++) {
    blob_t* blob = (blob_t*) get_dict(all_hashes, t->blobs[i]);
    cx += draw_blob(renderer, blob, size, cx, y+size*2, childx+i, childy+i, text);
    if (t->nb_tree != 0 || i != t->nb_blob-1) cx += size;
  }
  for (int i=0; i<t->nb_tree; i++) {
    tree_t* tree = (tree_t*) get_dict(all_hashes, t->trees[i]);
    cx += draw_tree(renderer, all_hashes, tree, size, cx, y+size*2, childx+t->nb_blob+i, childy+t->nb_blob+i, text);
    if (i != t->nb_tree-1) cx += size;
  }

  SDL_Rect test_rect;
  test_rect.x = (int) ((cx-x)/2 + x) - size/2;
  test_rect.y = y;
  test_rect.w = size;
  test_rect.h = size;
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &test_rect);

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  if (in_rect(mouseX, mouseY, test_rect)) {
    *text = t->name;
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for (int i=0; i<t->nb_blob+t->nb_tree; i++) {
    SDL_RenderDrawLine(renderer, test_rect.x+size/2, test_rect.y+size/2, childx[i]+size/2, childy[i]+size/2);
  }

  free(childx);
  free(childy);

  if (prx != NULL) *prx = test_rect.x;
  if (pry != NULL) *pry = test_rect.y;
  return cx-x;
}

int draw_commit(SDL_Renderer* renderer, dict_t* all_hashes, commit_t* c, int size, int x, int y, int* prx, int* pry, char** text) {
  tree_t* tree = (tree_t*) get_dict(all_hashes, c->tree);
  int csize = draw_tree(renderer, all_hashes, tree, size, x, y+size*2, NULL, NULL, text);

  SDL_Rect test_rect;
  test_rect.x = x + (int) csize/2 - size/2;
  test_rect.y = y;
  test_rect.w = size;
  test_rect.h = size;
  SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
  SDL_RenderFillRect(renderer, &test_rect);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawLine(renderer, test_rect.x+size/2, test_rect.y+size/2, test_rect.x+size/2, y+size*2+size/2);
  if (prx != NULL) *prx = test_rect.x;
  if (pry != NULL) *pry = test_rect.y;

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  if (in_rect(mouseX, mouseY, test_rect)) {
    *text = c->hash;
  }

  return size;
}


int window_commit_content(int* window_to_draw, SDL_Renderer *renderer, dict_t *all_hashes, commit_t *c, TTF_Font* font) {
  int x = 100;
  int y = 100;
  int size = 50;
  SDL_bool is_running = SDL_TRUE;
  SDL_Event event;
  keys_t *keys = init_keys();

  SDL_Color color = {0, 0, 0, 255};
  SDL_Surface *surfaceTitle = TTF_RenderUTF8_Solid(font, c->hash, color);
  SDL_Texture *textureTitle = SDL_CreateTextureFromSurface(renderer, surfaceTitle);
  SDL_Rect *title_rect = create_rect_from_text(font, c->hash, 1, 30, 30);

  while (is_running) {
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);

    char* text = NULL;
    draw_commit(renderer, all_hashes, c, size, x, y, NULL, NULL, &text);
    if (text != NULL) {
      draw_text_at_mouse(renderer, font, text);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, textureTitle, NULL, title_rect);

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
      case SDLK_ESCAPE :
        is_running = SDL_FALSE;
        break;
      default:
        break;
      }
    }

    SDL_RenderPresent(renderer);
  }
  free_keys(keys);

  SDL_FreeSurface(surfaceTitle);
  SDL_DestroyTexture(textureTitle);

  *window_to_draw = 1;

  return 0;
}
