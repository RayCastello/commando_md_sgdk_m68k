// commando_rom.c - the Z80 main-CPU program ROM image (0x0000-0xBFFF).
//
// TODO(Ray): replace this zero-filled placeholder with your own dump
// of Commando's main-CPU ROMs, concatenated in address order to
// exactly Z80_ROM_SIZE (0xC000 = 49152) bytes. Until then the
// interpreter just runs through zero bytes (NOP) forever - proves
// the plumbing boots without crashing, nothing more.
//
// `const` puts this in ROM (ROM_END region) rather than eating into
// the Mega Drive's 64KB of work RAM.

#include "z80core.h"

const u8 z80_rom_data[Z80_ROM_SIZE] = { 0 };
