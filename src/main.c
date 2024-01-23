#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

#include "chip.c"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int LOGICAL_WIDTH = 64;
const int LOGICAL_HEIGHT = 32;

int main() {
  int init_res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  if(init_res < 0) {
    printf("SDL could not be initialized\n"
           "SDL_Error: %s\n", SDL_GetError());
    return init_res;
  }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
  // Disable compositor bypass
  if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
    printf("SDL could not disable compositor bypass\n");
    return 0;
  }
#endif

  Uint32 window_flags = SDL_WINDOWPOS_CENTERED | SDL_WINDOW_RESIZABLE;
  SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator", window_flags, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if(!renderer) {
    printf("renderer could not be created\n"
           "SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  if (SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT)) {
    printf("failed to set render logical size\n"
           "SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  Chip chip;
  bool quit = false;

  while(!quit) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        quit = true;
        break;
      }

      if (event.type != SDL_KEYDOWN) {
        continue;
      }

      const char* key = SDL_GetKeyName(event.key.keysym.sym);
      handle_input(&chip, key);
    }
    
    advance(&chip);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
