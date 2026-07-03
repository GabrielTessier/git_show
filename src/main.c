
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <unistd.h>

#include "dict.h"
#include "view.h"
#include "window_commit_content.h"
#include "window_list_commit.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage : %s path/to/repo\n", argv[0]);
    return 1;
  }
  if (TTF_Init() < 0) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    return 1;
  }
  TTF_Font *liberation = TTF_OpenFont("fonts/liberation-fonts-ttf-2.1.5/LiberationSerif-Regular.ttf", 24);
  if (!liberation) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    TTF_Quit();
    return 1;
  }

  if (chdir(argv[1]) < 0) {
    perror(argv[1]);
    TTF_CloseFont(liberation);
    TTF_Quit();
    return 1;
  }
  
  dict_t* all_hashes = init_dict();;
  char* commit_hash = get_hashes(all_hashes);
  commit_t* HEAD = (commit_t*) get_dict(all_hashes, commit_hash);
  //print_commit(all_hashes, c, 0);

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    free(commit_hash);
    free_dict(all_hashes, free_elem);
    TTF_CloseFont(liberation);
    TTF_Quit();
    return 1;
  }
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_DisplayMode mode;
  if (SDL_GetCurrentDisplayMode(0, &mode) < 0) {
    fprintf(stderr, "SDL_GetCurrentDisplayMode: %s\n", SDL_GetError());
    SDL_Quit();
    free(commit_hash);
    free_dict(all_hashes, free_elem);
    TTF_CloseFont(liberation);
    TTF_Quit();
    return 1;
  }
  //int screen_width = mode.w;
  //int screen_height = mode.h;
  int window_width = 1000;
  int window_height = 1000;
  if (SDL_CreateWindowAndRenderer(window_width, window_height, SDL_WINDOW_SHOWN,
                                  &window, &renderer) < 0) {
    fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
    SDL_Quit();
    free(commit_hash);
    free_dict(all_hashes, free_elem);
    TTF_CloseFont(liberation);
    TTF_Quit();
    return EXIT_FAILURE;
  }

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

  TTF_CloseFont(liberation);
  TTF_Quit();

  free_dict(all_hashes, free_elem);
  free(commit_hash);

  return 0;
}
