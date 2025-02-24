
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <stdlib.h>

#include "dict.h"
#include "view.h"
#include "window.h"


int draw_blob(SDL_Renderer* renderer, blob_t* b, int size, int x, int y, int* prx, int* pry) {
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

  return size;
}

int draw_tree(SDL_Renderer* renderer, dict_t* all_hashes, tree_t* t, int size, int x, int y, int* prx, int* pry) {
//  printf("%s (%s)\n", t->name, t->hash);

  int* childx = (int*) malloc(sizeof(int)*(t->nb_blob+t->nb_tree));
  int* childy = (int*) malloc(sizeof(int)*(t->nb_blob+t->nb_tree));
  int cx = x;
  for (int i=0; i<t->nb_blob; i++) {
    blob_t* blob = (blob_t*) get_dict(all_hashes, t->blobs[i]);
    cx += draw_blob(renderer, blob, size, cx, y+size*2, childx+i, childy+i);
    if (t->nb_tree != 0 || i != t->nb_blob-1) cx += size;
  }
  for (int i=0; i<t->nb_tree; i++) {
    tree_t* tree = (tree_t*) get_dict(all_hashes, t->trees[i]);
    cx += draw_tree(renderer, all_hashes, tree, size, cx, y+size*2, childx+t->nb_blob+i, childy+t->nb_blob+i);
    if (i != t->nb_tree-1) cx += size;
  }

  SDL_Rect test_rect;
  test_rect.x = (int) ((cx-x)/2 + x) - size/2;
  test_rect.y = y;
  test_rect.w = size;
  test_rect.h = size;
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &test_rect);

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

int draw_commit(SDL_Renderer* renderer, dict_t* all_hashes, commit_t* c, int size, int x, int y, int* prx, int* pry) {
  tree_t* tree = (tree_t*) get_dict(all_hashes, c->tree);
  int csize = draw_tree(renderer, all_hashes, tree, size, x, y+size*2, NULL, NULL);

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
  return size;
}
