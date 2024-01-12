struct Chip {
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

  /// Program counter, can't be used by programs.
  unsigned short _pc;

  /// Stack counter, can't be used by programs.
  unsigned char _sc;
};

