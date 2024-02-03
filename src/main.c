#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

#include "chip.h"
#include "environment.h"

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please provide a path to a ROM.\n");
    return -1;
  }

  char* rom_path = argv[1];
  FILE *rom_file = fopen(rom_path, "r");
  if (!rom_file) {
    printf("Can't open ROM file at path: %s\n", rom_path);
    return -1;
  }

  Chip chip = new_chip();

  fread(&chip.memory[STARTING_ADDRESS], sizeof(chip.memory) - STARTING_ADDRESS, 1, rom_file);
  
  int init_res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

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

  if (SDL_RenderSetLogicalSize(renderer, RENDER_WIDTH, RENDER_HEIGHT)) {
    printf("failed to set render logical size\n"
           "SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  Environment environment = {
    renderer
  };

  clear_screen(&environment);
  render_screen(&environment);

  bool quit = false;
  Uint64 last_counter = SDL_GetPerformanceCounter();

  while(!quit) {
    Uint64 counter = SDL_GetPerformanceCounter();
    float delta = (counter - last_counter) / (float) SDL_GetPerformanceFrequency();
    last_counter = counter;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        quit = true;
        break;
      }

      if (event.type == SDL_KEYDOWN) {
        const char* key = SDL_GetKeyName(event.key.keysym.sym);
        handle_input(&chip, key, true);
      } else if (event.type == SDL_KEYUP) {
        const char* key = SDL_GetKeyName(event.key.keysym.sym);
        handle_input(&chip, key, false);
      }
    }
    
    advance(&chip, &environment, delta);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
