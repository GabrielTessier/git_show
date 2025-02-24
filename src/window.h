
#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL_render.h>
#include "view.h"

int draw_blob(SDL_Renderer* renderer, blob_t* b, int size, int x, int y, int* prx, int* pry);
int draw_tree(SDL_Renderer *renderer, dict_t *all_hashes, tree_t *t, int size,
int x, int y, int *prx, int *pry);
int draw_commit(SDL_Renderer* renderer, dict_t* all_hashes, commit_t* c, int size, int x, int y, int* prx, int* pry);


#endif // WINDOW_H
