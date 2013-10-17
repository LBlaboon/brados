#include <stddef.h>
#include <stdint.h>

// Make sure we are using the right compiler
#if defined(__linux__)
#error "You are using the wrong compiler."
#endif

// Color constants
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15
};

// Create a VGA color
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

// Create a VGA character
uint16_t make_vgaEntry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

// Calculate the length of a string
size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len] != 0)
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

size_t term_row, term_col;
uint8_t term_color;
uint16_t *term_buf;

// Initialize the terminal
void term_init()
{
	term_row = term_col = 0;
	term_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	term_buf = (uint16_t *) 0xB8000;
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			term_buf[index] = make_vgaEntry(' ', term_color);
		}
	}
}

// Set the terminal color
void term_setColor(uint8_t color)
{
	term_color = color;
}

// Place a character in the buffer
void term_putEntryAt(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	term_buf[index] = make_vgaEntry(c, color);
}

// Scroll down 1 line
void term_scroll()
{
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			const size_t indexBelow = (y - 1) * VGA_WIDTH + x;
			term_buf[indexBelow] = term_buf[index];
		}
	}
	
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		term_buf[index] = ' ';
	}
}

// Write a character to the terminal
void term_putChar(char c)
{
	if (c == '\n')
		term_col = VGA_WIDTH - 1;
	else
		term_putEntryAt(c, term_color, term_col, term_row);
	
	if (++term_col == VGA_WIDTH) {
		term_col = 0;
		if (++term_row == VGA_HEIGHT) {
			term_row--;
			term_scroll();
		}
	}
}

// Write a string to the terminal
void term_writeStr(const char *data)
{
	size_t dataLen = strlen(data);
	for (size_t i = 0; i < dataLen; i++)
		term_putChar(data[i]);
}

// Main kernel function
void brados_main()
{
	term_init();
	
	// Test printing and newlines
	term_writeStr("Welcome to BRaDOS!\n");
	term_writeStr("written by L. Bradley LaBoon\n\n");
	
	// Test line wrapping
	term_writeStr("The Laughing Man always says: 'I thought what I'd do was I'd pretend I was one of those deaf mutes.'\n\n");
	
	// Test colors
	term_setColor(make_color(COLOR_BLACK, COLOR_WHITE));
	term_writeStr("This text should be inverse.\n");
	term_setColor(make_color(COLOR_LIGHT_BLUE, COLOR_GREEN));
	term_writeStr("This text should be colorful.\n\n");
	
	// Test scrolling
	term_setColor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
	for (size_t i = 0; i < 20; i++) {
		term_writeStr("Printing some lines.\n");
		for (size_t j = 0; j < 100000000; j++);
	}
	
	term_writeStr("Done.");
}
