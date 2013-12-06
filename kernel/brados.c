#include <stddef.h>

#include <brados/string.h>
#include <video/vga.h>

// Make sure we are using the right compiler
#if defined(__linux__)
#error "You are using the wrong compiler."
#endif

// Test various VGA functionality
void vgaTests(struct vgastate *term)
{
	// Test line wrapping
	term_writeStr(term, "Welcome to the desert of the real. I thought what I'd do was I'd pretend I was one of those deaf mutes.\n\n");
	
	// Test colors
	term_setColor(term, make_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
	term_writeStr(term, "This text should be inverse.\n");
	term_setColor(term, make_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_GREEN));
	term_writeStr(term, "This text should be colorful.\n\n");
	
	// Test scrolling
	term_setColor(term, make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	for (size_t i = 0; i < 20; i++) {
		term_writeStr(term, "Printing some lines.\n");
		for (volatile size_t j = 0; j < 100000000; j++);
	}
}

// Main kernel function
void brados_main()
{
	struct vgastate term;
	term_init(&term);
	
	// Welcome message
	term_writeStr(&term, "Welcome to BRaDOS!\n");
	term_writeStr(&term, "written by L. Bradley LaBoon\n\n");
	
	vgaTests(&term);
	
	term_writeStr(&term, "Done.");
}
