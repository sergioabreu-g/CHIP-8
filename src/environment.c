#include "environment.h"
#include <SDL2/SDL_render.h>

void clear_screen(Environment *self) {
  SDL_RenderClear(self->_renderer);
}

void draw_pixel(Environment *self, int x, int y) {
  SDL_RenderDrawPoint(self->_renderer, x, y);
}
