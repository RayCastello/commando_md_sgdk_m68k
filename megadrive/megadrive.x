/* megadrive.x - link script for a bare-metal MD ROM. Same shape as
 * any raw m68k-elf link for this target: header+vectors first,
 * everything else follows, then a BSS section that the linker sizes
 * but that occupies no space in the ROM image itself.
 */

OUTPUT_FORMAT(elf32-m68k)
ENTRY(_start)

MEMORY
{
	rom (rx) : ORIGIN = 0x00000000, LENGTH = 4M
	ram (rwx) : ORIGIN = 0x00FF0000, LENGTH = 64K
}

SECTIONS
{
	.text 0x00000000 :
	{
		*(.text)
		*(.rodata*)
		. = ALIGN(2);
	} > rom

	ROM_END = .;

	.bss (0x00FF0000) (NOLOAD) :
	{
		*(.bss)
		*(COMMON)
	} > ram
}
