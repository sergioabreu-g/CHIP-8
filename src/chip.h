#ifndef CHIP_H
#define CHIP_H

#include <stdbool.h>
#include "environment.h"

static const int CLOCK_FREQUENCY = 500;
static const float CLOCK_PERIOD = 1.0/CLOCK_FREQUENCY;
static const short STARTING_ADDRESS = 0x600;

typedef unsigned short Instruction;

typedef struct Chip {
  /// General purpose registers.
  unsigned char regs[16];

  /// Delay register.
  unsigned char delay_reg;

  /// Sound register.
  unsigned char sound_reg;

  /// I register, mostly used to store memory adresses.
  unsigned short I_reg;

  /// Memory of 4096 bytes, bytes up to 512 are reserved for the interpreter.
  unsigned char memory[4096];

  /// Stack.
  unsigned short stack[16];

  /// Program counter, can't be used by programs. Most programs start at 0x600.
  unsigned short pc;

  /// Stack counter, can't be used by programs.
  unsigned char sc;

  /// Used to accumulate delta times when advancing the emulation.
  float delta_accumulator;

  /// Used to accumulate delta times to update the 60hz registers.
  float delta_accumulator_60hz;

  /// An array that contains which keys are currently pressed
  bool keys_pressed[16];

  /// Used by the Fx0A instruction to wait for a key press
  bool waiting_for_key_press;

  /// The first key pressed since it started waiting for a key press.
  /// 0x7F means no key was yet pressed.
  char first_key_pressed;
} Chip;

Chip new_chip();
char map_key_to_hex(const char* key);
void execute_instruction(Chip* chip, Environment* environment, const Instruction instruction);
void advance(Chip* chip, Environment *environment, float delta);
void handle_input(Chip* chip, const char* key, bool down);

#endif
