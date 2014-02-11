#include <stddef.h>
#include <stdint.h>

#include <brados/printk.h>
#include <brados/string.h>
#include <video/vga.h>
#include <multiboot.h>

// Make sure we are using the right compiler
#if defined(__linux__)
#error "You are using the wrong compiler."
#endif

// Main kernel function
void brados_main(uint32_t multiMagic, uint32_t multiAddr)
{
	// Initialize VGA terminal
	struct vgastate term;
	term_init(&term);
	
	// Welcome message
	printk(&term, "Welcome to BRaDOS!\n");
	printk(&term, "written by L. Bradley LaBoon\n\n");
	
	// Get multiboot info
	if (multiMagic != 0x2BADB002) {
		printk(&term, "Error! Multiboot header not present. Quitting.\n");
		return;
	}
	struct multiboot_header *multiHead = (struct multiboot_header *) multiAddr;
	if ((multiHead->flags >> 6) % 2 != 1) {
		printk(&term, "Error! Memory map not available from bootloader. Quitting.\n");
		return;
	}
	struct multiboot_mmap *memmap = (struct multiboot_mmap *) multiHead->mmap_addr;
	
	// Print memory map
	printk(&term, "Memory map:\n");
	int numEntries = multiHead->mmap_length / sizeof(struct multiboot_mmap);
	uint64_t lastAddr = 0;
	for (int i = 0; i < numEntries; i++) {
		if (memmap[i].base_addr != lastAddr) {
			printk(&term, "%llx - %llx: Unknown\n", lastAddr, memmap[i].base_addr - 1);
			lastAddr = memmap[i].base_addr;
		}
		
		printk(&term, "%llx - %llx: ", memmap[i].base_addr, memmap[i].base_addr + memmap[i].length - 1);
		if (memmap[i].type == 1)
			printk(&term, "Available\n");
		else
			printk(&term, "Reserved\n");
		
		lastAddr = memmap[i].base_addr + memmap[i].length;
	}
	
	
	// Done
	term_writeStr(&term, "\nDone.");
}
