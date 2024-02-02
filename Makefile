chip-8: src/*
	gcc src/*.c -o chip-8 `sdl2-config --cflags --libs`
