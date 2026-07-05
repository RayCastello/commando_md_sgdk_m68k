| =====================================================================
| z80_defs.inc - Z80 interpreter state layout, flag bits, shared macros
| Part of the Commando Mega Drive port (Option C: interpreted main CPU
| + real Z80 coprocessor for sound). Pure 68000 - no 68020 opcodes.
| =====================================================================

	.ifndef	Z80_DEFS_INC
Z80_DEFS_INC = 1

Z80_ROM_SIZE = 0xC000

| ---- Z80 F register bit layout (bit-exact with real hardware) ----
FLAG_C  = 0x01
FLAG_N  = 0x02
FLAG_PV = 0x04
FLAG_X  = 0x08		| undocumented, = bit3 of result
FLAG_H  = 0x10
FLAG_Y  = 0x20		| undocumented, = bit5 of result
FLAG_Z  = 0x40
FLAG_S  = 0x80

| ---- Z80 register state block (word pairs, big-endian = high:low
|      matches Z80 pairing directly, so BC/DE/HL/AF/IX/IY/SP/PC/IR
|      can all be read/written as .w for pair ops, or +0/+1 byte
|      offset for the individual 8 bit regs) ----
	.bss
	.align	2
z80_af:		.ds.w	1	| +0 = A   +1 = F
z80_bc:		.ds.w	1	| +0 = B   +1 = C
z80_de:		.ds.w	1	| +0 = D   +1 = E
z80_hl:		.ds.w	1	| +0 = H   +1 = L
z80_af2:	.ds.w	1
z80_bc2:	.ds.w	1
z80_de2:	.ds.w	1
z80_hl2:	.ds.w	1
z80_ix:		.ds.w	1
z80_iy:		.ds.w	1
z80_sp:		.ds.w	1
z80_pc:		.ds.w	1
z80_ir:		.ds.w	1	| +0 = I   +1 = R (R not cycle-accurate, just incremented per fetch)
z80_iff1:	.ds.b	1
z80_iff2:	.ds.b	1
z80_im:		.ds.b	1
z80_halted:	.ds.b	1
z80_int_pending: .ds.b	1	| set by 68000 side when it wants to raise a Z80 maskable /INT
z80_nmi_pending: .ds.b	1	| set by 68000 side to raise a Z80 /NMI (typical vblank hookup)
z80_int_data:	.ds.b	1	| data byte device would place on bus for IM0/IM2 (default 0xFF)
z80_cycles_lo:	.ds.w	1	| running cycle counter, low word (for frame budget)
z80_cycles_hi:	.ds.w	1
	.even

| convenience byte accessors (documented above, used directly in code
| as z80_a, z80_f, etc. via symbol+offset - GNU as accepts this)
z80_a  = z80_af
z80_f  = z80_af+1
z80_b  = z80_bc
z80_c  = z80_bc+1
z80_d  = z80_de
z80_e  = z80_de+1
z80_h  = z80_hl
z80_l  = z80_hl+1
z80_i  = z80_ir
z80_r  = z80_ir+1

| ---- macro: build Z80 flags (S Z PV C Y X) from 68000 SR after an
| ADD/SUB/ADDX/SUBX on register `res` (byte or word - pass size letter).
| Z is computed with TST (robust even after ADDX/SUBX, whose own Z bit
| has the classic "sticky" 68k semantics we don't want here).
| Result -> d3.b (N flag of Z80, i.e. bit1, is NOT set here - caller
| ORs in FLAG_N afterwards for subtract ops). Trashes d2.
	.macro	Z80_FLAGS_FROM_SR res,sz
	move	sr,d2
	moveq	#0,d3
	btst	#3,d2			| 68k N = sign
	beq.s	La\@
	bset	#7,d3
La\@:
	tst.\sz	\res
	bne.s	Lb\@
	bset	#6,d3
Lb\@:
	btst	#1,d2			| 68k V = signed overflow == Z80 PV for arith
	beq.s	Lc\@
	bset	#2,d3
Lc\@:
	btst	#0,d2			| 68k C
	beq.s	Ld\@
	bset	#0,d3
Ld\@:
	move.b	\res,d2
	and.b	#0x28,d2
	or.b	d2,d3
	.endm

| carry-in setup for ADC/SBC style ops: sets 68k X flag to match Z80 C
	.macro	Z80_CARRYIN_TO_X
	move.b	z80_f,d2
	and.b	#FLAG_C,d2
	beq.s	Le\@
	ori	#0x10,ccr
	bra.s	Lf\@
Le\@:
	andi	#0xEF,ccr
Lf\@:
	.endm

| ---- macro: Z80 half-carry (bit3->bit4 carry/borrow), works for both
| add and sub via the classic XOR trick: H = bit4 of (a ^ b ^ result)
| a=d4.b b=d5.b result=d1.b -> ORs FLAG_H into d3 (the flags being built)
	.macro	Z80_HALFCARRY a,b,result
	move.b	\a,d2
	eor.b	\b,d2
	eor.b	\result,d2
	and.b	#0x10,d2
	beq.s	Lj\@
	bset	#4,d3
Lj\@:
	.endm

| ---- macro: 16-bit parity of a byte (for AND/OR/XOR PV flag) ----
| uses PARITY_TABLE (256 bytes, built in z80_interp.68k). input d1.b,
| ORs FLAG_PV into d3 if EVEN parity (Z80 convention: PV=1 => even parity)
	.macro	Z80_PARITY_INTO_D3
	moveq	#0,d2
	move.b	d1,d2
	lea	PARITY_TABLE,a0
	tst.b	(a0,d2.w)
	beq.s	Lk\@
	bset	#2,d3
Lk\@:
	.endm

	.endif
