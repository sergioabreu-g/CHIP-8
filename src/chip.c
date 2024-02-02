#include "instructions.c"
#include <stdbool.h>
#include <stdio.h>

Chip new_chip() {
  Chip chip = { .pc = STARTING_ADDRESS };
  chip.memory[0x601] = 0xE0;
  chip.first_key_pressed = 0x7F;
  return chip;
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
    (*instruction_functions[instruction >> 12])(chip, environment, instruction);
}

void advance(Chip* chip, Environment *environment, float delta) {
  chip->delta_accumulator += delta;
  chip->delta_accumulator_60hz += delta;

  while (chip->delta_accumulator_60hz > (float) 1/60) {
    chip->delta_accumulator_60hz -= (float) 1/60;

    if (chip->delay_reg > 1) {
      chip->delay_reg -= 1;
    }

    if (chip->sound_reg > 1) {
      chip->sound_reg -= 1;
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
  char key_hex = map_key_to_hex(key);

  if (key_hex == ' ') {
    return;
  }

  chip->keys_pressed[key_hex] = down;

  if (chip->waiting_for_key_press && down && chip->first_key_pressed == 0x7F) {
    chip->first_key_pressed = key_hex;
  }
}
