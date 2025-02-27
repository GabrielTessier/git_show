
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "dict.h"
#include "view.h"
#include "window_commit_content.h"
#include "window_list_commit.h"

int main() {
  dict_t* all_hashes = init_dict();;
  char* commit_hash = get_hashes(all_hashes);
  commit_t* HEAD = (commit_t*) get_dict(all_hashes, commit_hash);
  //print_commit(all_hashes, c, 0);

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

  TTF_Init();
  TTF_Font *liberation = TTF_OpenFont("fonts/liberation-fonts-ttf-2.1.5/LiberationSerif-Regular.ttf", 24);

  int window_to_draw = 1;
  commit_t* commit = HEAD;

  while (1) {
    switch (window_to_draw) {
    case 1:
      window_list_commit(&window_to_draw, renderer, all_hashes, HEAD, liberation, &commit);
      break;
    case 2:
      window_commit_content(&window_to_draw, renderer, all_hashes, commit, liberation);
      break;
    default:
      goto end;
    }
  }
 end:
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  free_dict(all_hashes, free_elem);
  free(commit_hash);

  return 0;
}
