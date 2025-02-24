
#include <SDL2/SDL.h>

#include "dict.h"
#include "view.h"
#include "window.h"
#include "sdl_utils.h"

int main() {
  dict_t* all_hashes = init_dict();;
  char* commit_hash = get_hashes(all_hashes);
  commit_t* c = (commit_t*) get_dict(all_hashes, commit_hash);
  print_commit(all_hashes, c, 0);

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_DisplayMode mode;
  SDL_GetCurrentDisplayMode(0, &mode);
  //int screen_width = mode.w;
  //int screen_height = mode.h;
  int window_width = 1000;
  int window_height = 1000;
  SDL_CreateWindowAndRenderer(window_width, window_height, SDL_WINDOW_SHOWN, &window, &renderer);

  SDL_bool is_running = SDL_TRUE;
  SDL_Event event;

  int x = 100;
  int y = 100;
  int size = 50;

  keys_t* keys = init_keys();

  while (is_running) {
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);

    draw_commit(renderer, all_hashes, c, size, x, y, NULL, NULL);

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
        size += 10;
        break;
      case SDLK_DOWN:
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

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  free_keys(keys);

  free_dict(all_hashes, free_elem);
  free(commit_hash);

  return 0;
}
