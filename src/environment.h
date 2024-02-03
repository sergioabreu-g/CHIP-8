#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdbool.h>
#include <SDL2/SDL_render.h>

// This is an attempt at decoupling everything from the chip logic,
// but C is a horrible language so this is not great in any sense.
typedef struct Environment {  
  SDL_Renderer *_renderer;
} Environment;

typedef struct Point {  
  int x, y;
} Point;

void clear_screen(Environment* self);
void draw_pixel(Environment* self, int x, int y, bool clear);
void render_screen(Environment *self);

#endif
