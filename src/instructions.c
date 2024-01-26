#include "chip.h"
#include <stdio.h>

typedef unsigned short Instruction;

void instruction_0xxx(Chip *chip, const Instruction instruction) {
  switch (instruction) {
    case 0x00E0:
      printf("clear\n");
      break;
    case 0x00EE:
      chip->pc = chip->regs[chip->stack[chip->sc]];
      chip->sc -= 1;
      break;
    default:
      // 0nnn instructions are ignored by modern interpreters
      break;
  }
}

// This is better than doing a switch with all of the instructions at once,
// though not by that much xD.
static void (*instruction_functions[16]) (Chip*, const Instruction) = {
  [0] = instruction_0xxx,
};
