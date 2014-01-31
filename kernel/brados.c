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
	term_writeStr(&term, "Welcome to BRaDOS!\n");
	term_writeStr(&term, "written by L. Bradley LaBoon\n\n");
	
	// VGA tests
	//term_test(&term);
	
	// Get multiboot info
	printk(&term, "Multiboot value: %x\n", multiMagic);
	if (multiMagic != 0x2BADB002) {
		term_writeStr(&term, "Error! Multiboot header not present. Quitting.\n");
		return;
	}
	printk(&term, "Multiboot address: %x\n", multiAddr);
	struct multiboot_header *multiHead = (struct multiboot_header *) multiAddr;
	printk(&term, "Flags: %x\nMem Lower: %x\nMem Upper: %x\n", multiHead->flags, multiHead->mem_lower, multiHead->mem_upper);
	char *bootName = (char *) multiHead->boot_loader_name;
	printk(&term, "Boot loader: %s\n\n", bootName);
	
	// Done
	term_writeStr(&term, "Done.");
}
