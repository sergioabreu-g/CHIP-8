chip-8: src/*
	gcc src/main.c -o chip-8 `sdl2-config --cflags --libs`
