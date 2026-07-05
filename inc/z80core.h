#ifndef _Z80CORE_H_
#define _Z80CORE_H_

#include <genesis.h>

// From z80_interp.68k - unchanged, still hand-written 68000 asm (kept
// as-is because it already assembles/works and the flag computation
// leans on borrowing the 68000's own CCR, which is awkward to express
// and slower if rewritten in C).
extern void Z80_INIT(void);
extern void Z80_RUN(u16 instructionBudget);

// Z80 register state block - must match the layout in z80_defs.inc
// exactly (same struct, same field order/sizes) since both the C side
// and the asm side read/write it directly by symbol name.
extern volatile u16 z80_af;
extern volatile u16 z80_bc;
extern volatile u16 z80_de;
extern volatile u16 z80_hl;
extern volatile u16 z80_sp;
extern volatile u16 z80_pc;
extern volatile u8  z80_iff1;
extern volatile u8  z80_iff2;
extern volatile u8  z80_im;
extern volatile u8  z80_halted;
extern volatile u8  z80_int_pending;
extern volatile u8  z80_nmi_pending;
extern volatile u8  z80_int_data;

// z80_rom_data: the 48KB (0xC000) main-CPU program ROM image. TODO(Ray):
// replace the placeholder definition in commando_rom.c with your own
// dump, exactly Z80_ROM_SIZE bytes, address order 0x0000-0xBFFF.
#define Z80_ROM_SIZE 0xC000
extern const u8 z80_rom_data[Z80_ROM_SIZE];

// Diagnostics left behind by Z80_UNIMPL if the core hits an opcode it
// doesn't implement (still just DD/FD - IX/IY - as of this port).
extern volatile u16 z80_trap_pc;
extern volatile u8  z80_trap_op1;
extern volatile u8  z80_trap_op2;
extern volatile u8  z80_trapped;

#endif
