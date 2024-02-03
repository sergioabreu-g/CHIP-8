#include "environment.h"
#include <SDL2/SDL_render.h>

void clear_screen(Environment *self) {
  SDL_SetRenderDrawColor(self->_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(self->_renderer);
}

void draw_pixel(Environment* self, int x, int y, bool clear) {
  Uint8 color = clear? 0xFF : 0x00;
  SDL_SetRenderDrawColor(self->_renderer, color, color, color, 0xFF);
  SDL_RenderDrawPoint(self->_renderer, x, y);
}

void render_screen(Environment *self) {
  SDL_RenderPresent(self->_renderer);
}
