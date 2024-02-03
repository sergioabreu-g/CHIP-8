chip-8: src/*
	gcc src/main.c src/chip.c src/environment.c -o chip-8 `sdl2-config --cflags --libs`
