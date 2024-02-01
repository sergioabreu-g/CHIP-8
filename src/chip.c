#include "instructions.c"
#include <stdio.h>

Chip new_chip() {
  Chip chip = { .pc = STARTING_ADDRESS };
  chip.memory[0x601] = 0xE0;
  return chip;
}

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

void execute_instruction(Chip* chip, Environment* environment, const Instruction instruction) {
    (*instruction_functions[instruction >> 12])(chip, environment, instruction);
}

void advance(Chip* chip, Environment *environment, float delta) {
  chip->delta_accumulator += delta;
  chip->delta_accumulator_60hz += delta;

  while (chip->delta_accumulator_60hz > (float) 1/60) {
    chip->delta_accumulator_60hz -= (float) 1/60;

    if (chip->s_regs[0] > 1) {
      chip->s_regs[0] -= 1;
    }

    if (chip->s_regs[1] > 1) {
      chip->s_regs[1] -= 1;
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

void handle_input(Chip* chip, const char* key) {
  char mapped_key = map_key(key);

  if (mapped_key == ' ') {
    return;
  }

  // TODO
}
