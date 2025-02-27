
#ifndef WINDOW_COMMIT_CONTENT_H
#define WINDOW_COMMIT_CONTENT_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

#include "dict.h"
#include "view.h"

int window_commit_content(int *window_to_draw, SDL_Renderer *renderer, dict_t *all_hashes, commit_t *c, TTF_Font* font);

#endif // WINDOW_COMMIT_CONTENT_H
