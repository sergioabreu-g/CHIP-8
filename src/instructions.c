#include <stdio.h>
#include <stdlib.h>
#include "chip.h"
#include "environment.h"

void instruction_0nn(Chip *chip, Environment *environment, const Instruction instruction) {
  switch (instruction) {
    case 0x00E0:
      clear_screen(environment);

      for (int x = 0; x < RENDER_WIDTH; x++) {
        for (int y = 0; y < RENDER_HEIGHT; y++) {
          chip->pixels[x][y] = false;
        }
      }

      render_screen(environment);
      break;
    case 0x00EE:
      chip->pc = chip->stack[chip->sc];
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
  unsigned char x = instruction >> 8 & 0x000F;
  if (chip->regs[x] == (instruction & 0x00FF)) {
    chip->pc += 2;
  }
}

void instruction_4xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  if (chip->regs[x] != (instruction & 0x00FF)) {
    chip->pc += 2;
  }
}

void instruction_5xy0(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  unsigned char y = instruction >> 4 & 0x000F;

  if (chip->regs[x] == chip->regs[y]) {
    chip->pc += 2;
  }
}

void instruction_6xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  chip->regs[x] = 0x00FF & instruction;
}

void instruction_7xkk(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  chip->regs[x] = chip->regs[x] + (instruction & 0x00FF);
}

void instruction_8xyn(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  unsigned char y = instruction >> 4 & 0x000F;
  
  switch (instruction & 0x000F) {
    case 0x0:
      chip->regs[x] = chip->regs[y];
      break;
    case 0x1:
      chip->regs[x] = chip->regs[x] | chip->regs[y];
      break;
    case 0x2:
      chip->regs[x] = chip->regs[x] & chip->regs[y];
      break;
    case 0x3:
      chip->regs[x] = chip->regs[x] ^ chip->regs[y];
      break;
    case 0x4: {
      unsigned short sum = (unsigned short) chip->regs[x] + chip->regs[y];
      chip->regs[0xF] = sum > 0x00FF? 1 : 0;
      chip->regs[x] = sum & 0x00FF;
      break;
    }
    case 0x5:
      chip->regs[0xF] = chip->regs[x] > chip->regs[y]? 1 : 0;
      chip->regs[x] = chip->regs[x] - chip->regs[y];
      break;
    case 0x6:
      chip->regs[0xF] = (chip->regs[x] & 0x1) == 0x1? 1 : 0;
      chip->regs[x] = chip->regs[x] / 2;
      break;
    case 0x7:
      chip->regs[0xF] = chip->regs[y] > chip->regs[x]? 1 : 0;
      chip->regs[x] = chip->regs[y] - chip->regs[x];
      break;
    case 0xE:
      chip->regs[0xF] = (chip->regs[x] & 0x80) == 0x80? 1 : 0;
      chip->regs[x] = chip->regs[x] * 2;
      break;
    default:
      break;
  }
}

void instruction_9xy0(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  unsigned char y = instruction >> 4 & 0x000F;

  if (chip->regs[x] != chip->regs[y]) {
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
  unsigned char x = instruction >> 8 & 0x000F;
  unsigned char random = rand() % 256;

  chip->regs[x]= random & (instruction & 0x00FF);
}

void instruction_Dxyn(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;
  unsigned char y = instruction >> 4 & 0x000F;
  unsigned char n = instruction & 0x000F;

  unsigned char x_origin = chip->regs[x];
  unsigned char y_origin = chip->regs[y];

  chip->regs[0xF] = 0;

  for (short sprite_y = 0; sprite_y < n; sprite_y++) {
    char byte = chip->memory[chip->I_reg + sprite_y];

    for (char sprite_x = 0; sprite_x < 8; sprite_x++) {
      char bit = 0x80 >> sprite_x;

      if ((byte & bit) != bit) {
        continue;
      }

      char screen_x = (x_origin + sprite_x) % RENDER_WIDTH;
      char screen_y = (y_origin + sprite_y) % RENDER_HEIGHT;

      bool clear = chip->pixels[screen_x][screen_y];

      draw_pixel(environment, screen_x, screen_y, clear);
      chip->pixels[screen_x][screen_y] = !clear;

      if (clear) {
        chip->regs[0xF] = 1;
      }
    }
  }

  render_screen(environment);
}

void instruction_Exnn(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;

  switch (instruction & 0x00FF) {
    case 0x9E:
      if (chip->keys_pressed[chip->regs[x]]) {
        chip->pc += 2;
      }
      break;
    case 0xA1:
      if (!chip->keys_pressed[chip->regs[x]]) {
        chip->pc += 2;
      }
      break;
    default:
      break;
  }
}

void instruction_Fxnn(Chip *chip, Environment *environment, const Instruction instruction) {
  unsigned char x = instruction >> 8 & 0x000F;

  switch (instruction & 0x00FF) {
    case 0x07:
      chip->regs[x] = chip->delay_reg;
      break;
    case 0x0A:
      if (!chip->waiting_for_key_press || chip->first_key_pressed == 0x7F) {
        chip->waiting_for_key_press = true;
      
        // Repeat this instruction until a key is pressed
        chip->pc -= 2;
      } else {
        chip->regs[x] = chip->first_key_pressed;
        chip->waiting_for_key_press = false;
        chip->first_key_pressed = 0x7F;
      }
      break;
    case 0x15:
      chip->delay_reg = chip->regs[x];
      break;
    case 0x18:
      chip->sound_reg = chip->regs[x];
      break;
    case 0x1E:
      chip->I_reg = chip->I_reg + chip->regs[x];
      break;
    case 0x29:
      chip->I_reg = chip->regs[x] * 5;
      break;
    case 0x33: {
      char number = chip->regs[x];

      for (short i = 2; i > 0; i++) {
        chip->memory[chip->I_reg + i] = number % 10;
        number /= 10;
      }

      break;
    }
    case 0x55:
      for (char i = 0; i < x; i++) {
        chip->memory[chip->I_reg + i] = chip->regs[i];
      }
      break;
    case 0x65:
      for (char i = 0; i < x; i++) {
        chip->regs[i] = chip->memory[chip->I_reg + i];
      }
      break;
    default:
      break;
  }
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
  [0xD] = instruction_Dxyn,
  [0xE] = instruction_Exnn,
  [0xF] = instruction_Fxnn,
};
