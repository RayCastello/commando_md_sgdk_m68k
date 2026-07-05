// commando_io.c - C rewrite of the Commando-specific memory bridge.
// Same map as before (see commando_map.68k in the raw-asm version for
// the full comment/source citation); this version calls SGDK's own
// tested joypad API instead of hand-rolled 6-button reading.
//
// Called from io_shim.68k, NOT directly by the Z80 core - see that
// file for the calling-convention note.

#include <genesis.h>
#include "z80core.h"

#define Z80_RAM_SIZE 0x4000

u8 z80_ram[Z80_RAM_SIZE];		// emulates C000-FFFF

u16 z80_scrollx;
u16 z80_scrolly;
u8  z80_soundlatch;
u8  z80_soundlatch_new;		// TODO: consumed by the (not yet
					// written) real Z80 sound driver
volatile u8 vdp_video_dirty;

static u8 pad1_state = 0xFF;
static u8 pad2_state = 0xFF;
static u8 dsw1_value = 0xFF;		// TODO: set real dip defaults once
static u8 dsw2_value = 0xFF;		// you know what they should be

// Call once per frame from main.c, before Z80_RUN.
void commando_update_pads(void)
{
    u16 j1 = JOY_readJoypad(JOY_1);
    u16 j2 = JOY_readJoypad(JOY_2);

    // TODO(Ray): placeholder bit mapping / polarity. Cross-check the
    // real IN0/IN1/IN2 bit assignment in commando_z80.asm (Capcom
    // boards of this era are typically active-low) before trusting
    // coin-up / attract-mode behaviour.
    pad1_state = 0xFF;
    if (j1 & JOY_UP)    pad1_state &= ~0x01;
    if (j1 & JOY_DOWN)  pad1_state &= ~0x02;
    if (j1 & JOY_LEFT)  pad1_state &= ~0x04;
    if (j1 & JOY_RIGHT) pad1_state &= ~0x08;
    if (j1 & JOY_A)     pad1_state &= ~0x10;
    if (j1 & JOY_B)     pad1_state &= ~0x20;
    if (j1 & JOY_START) pad1_state &= ~0x40;

    pad2_state = 0xFF;
    if (j2 & JOY_UP)    pad2_state &= ~0x01;
    if (j2 & JOY_DOWN)  pad2_state &= ~0x02;
    if (j2 & JOY_LEFT)  pad2_state &= ~0x04;
    if (j2 & JOY_RIGHT) pad2_state &= ~0x08;
    if (j2 & JOY_A)     pad2_state &= ~0x10;
    if (j2 & JOY_B)     pad2_state &= ~0x20;
}

// Called (via io_shim.68k) from the Z80 core for every read at a
// z80-address >= 0xC000.
u8 commando_io_read(u16 addr)
{
    switch (addr) {
        case 0xC000: return pad1_state;   // TODO confirm IN0/IN1/IN2 role split
        case 0xC001: return pad1_state;
        case 0xC002: return pad2_state;
        case 0xC003: return dsw1_value;
        case 0xC004: return dsw2_value;
        default:     return z80_ram[addr - 0xC000];
    }
}

// Called (via io_shim.68k) from the Z80 core for every write at a
// z80-address >= 0xC000. `value` only carries a meaningful low byte.
void commando_io_write(u16 addr, u16 value)
{
    u8 v = (u8)value;

    switch (addr) {
        case 0xC800:
            z80_soundlatch = v;
            z80_soundlatch_new = 1;
            return;
        // TODO: byte order (hi first vs lo first) for the scroll pair
        // is a guess - verify against commando_z80.asm's write order.
        case 0xC808: z80_scrollx = (u16)((z80_scrollx & 0x00FF) | (v << 8)); return;
        case 0xC809: z80_scrollx = (u16)((z80_scrollx & 0xFF00) | v);        return;
        case 0xC80A: z80_scrolly = (u16)((z80_scrolly & 0x00FF) | (v << 8)); return;
        case 0xC80B: z80_scrolly = (u16)((z80_scrolly & 0xFF00) | v);        return;
        default:
            z80_ram[addr - 0xC000] = v;
            if (addr >= 0xD000 && addr < 0xE000) vdp_video_dirty = 1;
            return;
    }
}

// TODO: real implementation. Needs (1) Commando's arcade tile/sprite
// ROMs converted to Mega Drive 4bpp format + a matching palette - an
// asset-side job, not engine code, and not started; (2) a mapping
// from arcade tile-number to Mega Drive VDP tile index; (3) pushing
// every changed cell in z80_ram[0x1000..0x2FFF] (== arcade D000-DFFF)
// to VDP plane A/B via VDP_setTileMapXY or a DMA queue write. Left as
// a stub so the project links and runs (showing nothing recognisable)
// today, rather than pretending this part is finished.
void commando_flush_video(void)
{
}
