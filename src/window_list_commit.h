
#ifndef WINDOW_LIST_COMMIT_H
#define WINDOW_LIST_COMMIT_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "dict.h"
#include "view.h"

int window_list_commit(int *window_to_draw, SDL_Renderer *renderer, dict_t *all_hashes, commit_t *c, TTF_Font* font, commit_t** commit_out);

#endif // WINDOW_LIST_COMMIT_H
