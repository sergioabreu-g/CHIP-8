#include <SDL2/SDL_video.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Chip {
  /// General purpose registers.
  unsigned char regs[16];

  /// Special registers: sound and timer.
  unsigned char s_regs[2];

  /// I register, mostly used to store memory adresses.
  unsigned short I_reg;

  /// Memory of 4096 bytes, bytes up to 512 are reserved for the interpreter.
  unsigned char memory[4096];

  /// Stack.
  unsigned short stack[16];

  /// Program counter, can't be used by programs.
  unsigned short _pc;

  /// Stack counter, can't be used by programs.
  unsigned char _sc;
};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
  int init_res = SDL_Init(SDL_INIT_VIDEO);

  if(init_res < 0)
  {
      printf("SDL could not be initialized!\n"
             "SDL_Error: %s\n", SDL_GetError());
      return init_res;
  }

  SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOW_RESIZABLE, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if(!renderer)
  {
      printf("Renderer could not be created!\n"
             "SDL_Error: %s\n", SDL_GetError());
      return -1;
  }

    // Event loop exit flag
    bool quit = false;

    // Event loop
    while(!quit)
    {
        SDL_Event e;

        // Wait indefinitely for the next available event
        SDL_WaitEvent(&e);

        // User requests quit
        if(e.type == SDL_QUIT)
        {
            quit = true;
        }

        // Initialize renderer color white for the background
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Clear screen
        SDL_RenderClear(renderer);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Destroy renderer
    SDL_DestroyRenderer(renderer);

  // Destroy window
  SDL_DestroyWindow(window);

  // Quit SDL
  SDL_Quit();

  return 0;
}
