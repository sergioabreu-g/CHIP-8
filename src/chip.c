#include <stdbool.h>
#include <stdio.h>
#include "chip.h"
#include "environment.h"
#include "instructions.c"

Chip new_chip() {
  Chip chip = { .pc = STARTING_ADDRESS, .first_key_pressed = 0x7F };
  setup_memory(&chip);
  return chip;
}

void setup_memory(Chip* chip) {
  // Hexadecimal sprites (ordered)
  chip->memory[0x0] = 0xF0;
  chip->memory[0x1] = 0x90;
  chip->memory[0x2] = 0x90;
  chip->memory[0x3] = 0x90;
  chip->memory[0x4] = 0xF0;
  
  chip->memory[0x5] = 0x20;
  chip->memory[0x6] = 0x60;
  chip->memory[0x7] = 0x20;
  chip->memory[0x8] = 0x20;
  chip->memory[0x9] = 0x70;

  chip->memory[0xA] = 0xF0;
  chip->memory[0xB] = 0x10;
  chip->memory[0xC] = 0xF0;
  chip->memory[0xD] = 0x80;
  chip->memory[0xE] = 0xF0;
  
  chip->memory[0xF] = 0xF0;
  chip->memory[0x10] = 0x10;
  chip->memory[0x11] = 0xF0;
  chip->memory[0x12] = 0x10;
  chip->memory[0x13] = 0xF0;

  chip->memory[0x14] = 0x90;
  chip->memory[0x15] = 0x90;
  chip->memory[0x16] = 0xF0;
  chip->memory[0x17] = 0x10;
  chip->memory[0x18] = 0x10;

  chip->memory[0x19] = 0xF0;
  chip->memory[0x1A] = 0x80;
  chip->memory[0x1B] = 0xF0;
  chip->memory[0x1C] = 0x10;
  chip->memory[0x1D] = 0xF0;

  chip->memory[0x1E] = 0xF0;
  chip->memory[0x1F] = 0x80;
  chip->memory[0x20] = 0xF0;
  chip->memory[0x21] = 0x90;
  chip->memory[0x22] = 0xF0;

  chip->memory[0x23] = 0xF0;
  chip->memory[0x24] = 0x10;
  chip->memory[0x25] = 0x20;
  chip->memory[0x26] = 0x40;
  chip->memory[0x27] = 0x40;

  chip->memory[0x28] = 0xF0;
  chip->memory[0x29] = 0x90;
  chip->memory[0x2A] = 0xF0;
  chip->memory[0x2B] = 0x90;
  chip->memory[0x2C] = 0xF0;

  chip->memory[0x2D] = 0xF0;
  chip->memory[0x2E] = 0x90;
  chip->memory[0x2F] = 0xF0;
  chip->memory[0x30] = 0x10;
  chip->memory[0x31] = 0xF0;

  chip->memory[0x32] = 0xF0;
  chip->memory[0x33] = 0x90;
  chip->memory[0x34] = 0xF0;
  chip->memory[0x35] = 0x90;
  chip->memory[0x36] = 0x90;
  
  chip->memory[0x37] = 0xE0;
  chip->memory[0x38] = 0x90;
  chip->memory[0x39] = 0xE0;
  chip->memory[0x3A] = 0x90;
  chip->memory[0x3B] = 0xE0;

  chip->memory[0x3C] = 0xF0;
  chip->memory[0x3D] = 0x80;
  chip->memory[0x3E] = 0x80;
  chip->memory[0x3F] = 0x80;
  chip->memory[0x40] = 0xF0;

  chip->memory[0x41] = 0xE0;
  chip->memory[0x42] = 0x90;
  chip->memory[0x43] = 0x90;
  chip->memory[0x44] = 0x90;
  chip->memory[0x45] = 0xE0;

  chip->memory[0x46] = 0xF0;
  chip->memory[0x47] = 0x80;
  chip->memory[0x48] = 0xF0;
  chip->memory[0x49] = 0x80;
  chip->memory[0x4A] = 0xF0;
  
  chip->memory[0x4B] = 0xF0;
  chip->memory[0x4C] = 0x80;
  chip->memory[0x4D] = 0xF0;
  chip->memory[0x4E] = 0x80;
  chip->memory[0x4F] = 0xF0;
}

/// Maps the key from the character pressed to its hexadecimal equivalent in Chip-8
char map_key_to_hex(const char* key) {
  switch (*key) {
    case '1': return 0x1;
    case '2': return 0x2;
    case '3': return 0x3;
    case '4': return 0xC;
    case 'q': return 0x4;
    case 'w': return 0x5;
    case 'e': return 0x6;
    case 'r': return 0xD;
    case 'a': return 0x7;
    case 's': return 0x8;
    case 'd': return 0x9;
    case 'f': return 0xE;
    case 'z': return 0xA;
    case 'x': return 0x0;
    case 'c': return 0xB;
    case 'v': return 0xF;
    default: return ' ';
  }
}

void execute_instruction(Chip* chip, Environment* environment, const Instruction instruction) {
  printf("instruction: %#4x\n", instruction);
  (*instruction_functions[instruction >> 12])(chip, environment, instruction);
}

void advance(Chip* chip, Environment *environment, float delta) {
  chip->delta_accumulator += delta;
  chip->delta_accumulator_60hz += delta;

  while (chip->delta_accumulator_60hz > (float) 1/60) {
    chip->delta_accumulator_60hz -= (float) 1/60;

    if (chip->delay_reg > 0) {
      chip->delay_reg -= 1;
    }

    if (chip->sound_reg > 0) {
      chip->sound_reg -= 1;
      printf("sound\n");
    }
  }

  while (chip->delta_accumulator > CLOCK_PERIOD) {
    chip->delta_accumulator -= CLOCK_PERIOD;

    const Instruction instruction = ((short) chip->memory[chip->pc] << 8) | chip->memory[chip->pc + 1];
    chip->pc += 2;

    if (chip->pc > sizeof(chip->memory)) {
      chip->pc = STARTING_ADDRESS;
    }
    
    execute_instruction(chip, environment, instruction);
  }
}

void handle_input(Chip* chip, const char* key, bool down) {
  const char lowered_key = tolower(*key);
  char key_hex = map_key_to_hex(&lowered_key);

  if (key_hex == ' ') {
    return;
  }

  chip->keys_pressed[key_hex] = down;

  if (chip->waiting_for_key_press && down && chip->first_key_pressed == 0x7F) {
    chip->first_key_pressed = key_hex;
  }
}
