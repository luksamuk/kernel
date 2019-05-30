OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
	/* Start sections at 1M */
	. = 1M;

	/* Multiboot header and .text */
	.text BLOCK(4K) : ALIGN(4K) {
	      *(.multiboot)
	      *(.text)
	      *(.irq)
	}

	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	/* Read-write data */
	.data BLOCK(4K) : ALIGN(4K) {
	      *(.data)
	}

	/* Uninitialized read-write data & stack */
	.bss BLOCK(4K) : ALIGN(4K) {
	      *(COMMON)
	      *(.bss)
	}
}
