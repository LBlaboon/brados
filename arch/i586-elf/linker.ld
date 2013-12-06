/* Kernel entry point */
ENTRY(_start)

/* Define the locations of the object file sections */
SECTIONS
{
	/* Sections begin at 1 MiB */
	. = 1M;
	
	/* The multiboot header needs to come first, followed by program code */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	
	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	
	/* Initialized read-write data */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	
	/* Uninitialized read-write data and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
	
	/* Add other sections here */
}
