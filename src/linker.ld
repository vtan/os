KERNEL_MEMORY_OFFSET = 0xFFFFFF8000000000;
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = KERNEL_MEMORY_OFFSET + 1M;
	_kernel_start = .;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text ALIGN(4K) : AT(ADDR(.text) - KERNEL_MEMORY_OFFSET)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_MEMORY_OFFSET)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - KERNEL_MEMORY_OFFSET)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_MEMORY_OFFSET)
	{
		*(COMMON)
		*(.bss)
	}
	_kernel_end = .;

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
