// main.c - Commando MD port, Option C (interpreted main CPU), SGDK shell.
//
// Everything SGDK already does for you (TMSS, VDP bring-up, vector
// table, joypad driver, DMA queue) replaces the hand-written
// header.68k/startup.68k/megadrive.x from the raw-asm version of this
// project - that boilerplate is gone on purpose, not forgotten.

#include <genesis.h>
#include "z80core.h"

extern void commando_update_pads(void);
extern void commando_flush_video(void);
extern volatile u8 vdp_video_dirty;
extern u8 z80_soundlatch_new;		// TODO: consume this once the real
					// Z80 sound driver exists

int main(bool hardReset)
{
    JOY_init();

    Z80_INIT();

    // TODO(Ray): confirm Commando's real main-CPU reset vector against
    // the ROM dump you drop into commando_rom.c - defaulting to 0x0000.
    z80_pc = 0x0000;
    z80_im = 1;			// harmless default; only matters if the
    				// main CPU ever uses maskable INT

    while (TRUE)
    {
        // TODO(Ray): confirm the main CPU really hooks vblank via NMI
        // and not IM1 (varies by Capcom board revision) before trusting
        // this. If it's INT-based, set z80_int_pending instead.
        z80_nmi_pending = 1;

        commando_update_pads();

        // Placeholder instruction budget, not a real T-state count -
        // see z80_interp.68k's header comment. Tune once something's
        // on screen and you can measure against real hardware timing.
        Z80_RUN(4000);

        if (vdp_video_dirty)
        {
            vdp_video_dirty = 0;
            commando_flush_video();
        }

        if (z80_trapped)
        {
            // The Z80 core hit something it doesn't implement (DD/FD
            // as of this port) and froze itself. Surface it instead of
            // silently hanging so you're not guessing why nothing moves.
            VDP_drawText("Z80 CORE TRAP - SEE z80_trap_pc/op1/op2", 2, 2);
            while (TRUE) SYS_doVBlankProcess();
        }

        SYS_doVBlankProcess();
    }

    return 0;
}
