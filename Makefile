chip-8: main.c
	gcc main.c -o chip-8 `sdl2-config --cflags --libs`
