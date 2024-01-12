chip-8: src/main.c
	gcc src/main.c -o chip-8 `sdl2-config --cflags --libs`
