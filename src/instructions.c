#include "chip.h"
#include "environment.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned short Instruction;

void instruction_0nn(Chip *chip, Environment *environment, const Instruction instruction) {
  switch (instruction) {
    case 0x00E0:
      clear_screen(environment);
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

void instruction_1nnn(Chip *chip, Environment *environment, const Instruction instruction) {
  chip->pc = 0x0FFF & instruction;
}

void instruction_2nnn(Chip *chip, Environment *environment, const Instruction instruction) {
  chip->sc += 1;
  chip->stack[chip->sc] = chip->pc;
  chip->pc = 0x0FFF & instruction;
}

void instruction_3xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg = instruction >> 8 & 0x000F;
  if (chip->regs[reg] == (instruction & 0x00FF)) {
    chip->pc += 2;
  }
}

void instruction_4xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg = instruction >> 8 & 0x000F;
  if (chip->regs[reg] != (instruction & 0x00FF)) {
    chip->pc += 2;
  }
}

void instruction_5xy0(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg_x = instruction >> 8 & 0x000F;
  unsigned char reg_y = instruction >> 4 & 0x000F;

  if (chip->regs[reg_x] == chip->regs[reg_y]) {
    chip->pc += 2;
  }
}

void instruction_6xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg = instruction >> 8 & 0x000F;
  chip->regs[reg] = 0x00FF & instruction;
}

void instruction_7xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg = instruction >> 8 & 0x000F;
  chip->regs[reg] = chip->regs[reg] + (instruction & 0x00FF);
}

void instruction_8xyn(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg_x = instruction >> 8 & 0x000F;
  unsigned char reg_y = instruction >> 4 & 0x000F;
  
  switch (instruction & 0x000F) {
    case 0x0:
      chip->regs[reg_x] = chip->regs[reg_y];
      break;
    case 0x1:
      chip->regs[reg_x] = chip->regs[reg_x] | chip->regs[reg_y];
      break;
    case 0x2:
      chip->regs[reg_x] = chip->regs[reg_x] & chip->regs[reg_y];
      break;
    case 0x3:
      chip->regs[reg_x] = chip->regs[reg_x] ^ chip->regs[reg_y];
      break;
    case 0x4: {
      unsigned short sum = (unsigned short) chip->regs[reg_x] + chip->regs[reg_y];
      chip->regs[0xF] = sum > 0x00FF? 1 : 0;
      chip->regs[reg_x] = sum & 0x00FF;
      break;
    }
    case 0x5:
      chip->regs[0xF] = chip->regs[reg_x] > chip->regs[reg_y]? 1 : 0;
      chip->regs[reg_x] = chip->regs[reg_x] - chip->regs[reg_y];
      break;
    case 0x6:
      chip->regs[0xF] = (chip->regs[reg_x] & 0x1) == 0x1? 1 : 0;
      chip->regs[reg_x] = chip->regs[reg_x] / 2;
      break;
    case 0x7:
      chip->regs[0xF] = chip->regs[reg_y] > chip->regs[reg_x]? 1 : 0;
      chip->regs[reg_x] = chip->regs[reg_y] - chip->regs[reg_x];
      break;
    case 0xE:
      chip->regs[0xF] = (chip->regs[reg_x] & 0x80) == 0x80? 1 : 0;
      chip->regs[reg_x] = chip->regs[reg_x] * 2;
      break;
    default:
      break;
  }
}

void instruction_9xy0(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg_x = instruction >> 8 & 0x000F;
  unsigned char reg_y = instruction >> 4 & 0x000F;

  if (chip->regs[reg_x] != chip->regs[reg_y]) {
    chip->pc += 2;
  }
}

void instruction_Annn(Chip *chip, Environment *environment, const Instruction instruction) {
  chip->I_reg = instruction & 0x0FFF;
}

void instruction_Bnnn(Chip *chip, Environment *environment, const Instruction instruction) {
  chip->pc = (instruction & 0x0FFF) + chip->regs[0];
}

void instruction_Cxkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char reg_x = instruction >> 8 & 0x000F;
  unsigned char random = rand() % 256;

  chip->regs[reg_x]= random & (instruction & 0x00FF);
}

void instruction_Dxkn(Chip *chip, Environment *environment, const Instruction instruction) {
  todo
}

// This is better than doing a switch with all of the instructions at once,
// though not by that much xD.
static void (*instruction_functions[16]) (Chip*, Environment*, const Instruction) = {
  [0x0] = instruction_0nn,
  [0x1] = instruction_1nnn,
  [0x2] = instruction_2nnn,
  [0x3] = instruction_3xkk,
  [0x4] = instruction_4xkk,
  [0x5] = instruction_5xy0,
  [0x6] = instruction_6xkk,
  [0x7] = instruction_7xkk,
  [0x8] = instruction_8xyn,
  [0x9] = instruction_9xy0,
  [0xA] = instruction_Annn,
  [0xB] = instruction_Bnnn,
  [0xC] = instruction_Cxkk,
  [0xD] = instruction_Dxkn,
};
