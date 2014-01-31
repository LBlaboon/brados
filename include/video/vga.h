#ifndef __brados_video_vga_h__
#define __brados_video_vga_h__

#include <stddef.h>
#include <stdint.h>

#define VGA_BUF 0xB8000

#define VGA_WIDTH	80
#define VGA_HEIGHT	24

// VGA State
struct vgastate {
	size_t term_row, term_col;
	uint8_t term_color;
	uint16_t *term_buf;
};

// Color constants
enum vga_color
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15
};

uint8_t make_color(enum vga_color fg, enum vga_color bg);
uint16_t make_vgaEntry(char c, uint8_t color);
void term_init(struct vgastate *state);
void term_putChar(struct vgastate *state, char c);
void term_putEntryAt(struct vgastate *state, char c, uint8_t color, size_t x, size_t y);
void term_scroll(struct vgastate *state);
void term_setColor(struct vgastate *state, uint8_t color);
void term_test(struct vgastate *term);
void term_writeStr(struct vgastate *state, const char *data);

#endif
