#include "chip.h"

char map_key(const char* key) {
  switch (*key) {
    case '1': return '1';
    case '2': return '2';
    case '3': return '3';
    case '4': return 'C';
    case 'q': return '4';
    case 'w': return '5';
    case 'e': return '6';
    case 'r': return 'D';
    case 'a': return '7';
    case 's': return '8';
    case 'd': return '9';
    case 'f': return 'E';
    case 'z': return 'A';
    case 'x': return '0';
    case 'c': return 'B';
    case 'v': return 'F';
  }

  return ' ';
}

void advance(Chip* chip) {
  
}

void handle_input(Chip* chip, const char* key) {
  char mapped_key = map_key(key);

  if (mapped_key == ' ') {
    return;
  }

  // TODO
}
