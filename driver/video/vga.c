#include <stddef.h>
#include <stdint.h>

#include <brados/string.h>
#include <video/vga.h>

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

// Initialize a VGA term
void term_init(struct vgastate *state)
{
	state->term_row = state->term_col = 0;
	state->term_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	state->term_buf = (uint16_t *) VGA_BUF;
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			state->term_buf[index] = make_vgaEntry(' ', state->term_color);
		}
	}
}

// Write a character to a VGA term
void term_putChar(struct vgastate *state, char c)
{
	if (c == '\n')
		state->term_col = VGA_WIDTH - 1;
	else
		term_putEntryAt(state, c, state->term_color, state->term_col, state->term_row);
	
	if (++state->term_col == VGA_WIDTH) {
		state->term_col = 0;
		if (++state->term_row == VGA_HEIGHT) {
			state->term_row--;
			term_scroll(state);
		}
	}
}

// Place a character in a VGA term's buffer
void term_putEntryAt(struct vgastate *state, char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	state->term_buf[index] = make_vgaEntry(c, color);
}

// Scroll the term down 1 line
void term_scroll(struct vgastate *state)
{
	// Move each row up
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			const size_t indexBelow = (y - 1) * VGA_WIDTH + x;
			state->term_buf[indexBelow] = state->term_buf[index];
		}
	}
	
	// Clear the last line
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		state->term_buf[index] = ' ';
	}
}

// Set VGA term color
void term_setColor(struct vgastate *state, uint8_t color)
{
	state->term_color = color;
}

// Write a string to a VGA term
void term_writeStr(struct vgastate *state, const char *data)
{
	size_t dataLen = strlen(data);
	for (size_t i = 0; i < dataLen; i++)
		term_putChar(state, data[i]);
}
