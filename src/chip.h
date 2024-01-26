const int CLOCK_FREQUENCY = 500;
const float CLOCK_PERIOD = 1.0/CLOCK_FREQUENCY;
const short STARTING_ADDRESS = 0x600;

typedef struct Chip {
  /// General purpose registers.
  unsigned char regs[16];

  /// Special registers: sound and timer.
  unsigned char s_regs[2];

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
} Chip;
