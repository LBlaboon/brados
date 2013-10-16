#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Make sure we are using the right compiler
#if defined(__linux__)
#error "You are using the wrong compiler."
#endif

// Color constants
enum vgaColor
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

uint8_t makeColor(enum vgaColor fg, enum vgaColor bg)
{
	return fg | bg << 4;
}

uint16_t makeVGAEntry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len] != 0)
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

size_t terminalRow, terminalCol;
uint8_t terminalColor;
uint16_t *terminalBuf;

void terminalInit()
{
	terminalRow = terminalCol = 0;
	terminalColor = makeColor(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminalBuf = (uint16_t *) 0xB8000;
	
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			const size_t index = y * VGA_WIDTH + x;
			terminalBuf[index] = makeVGAEntry(' ', terminalColor);
		}
	}
}

void terminalSetColor(uint8_t color)
{
	terminalColor = color;
}

void terminalPutEntryAt(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminalBuf[index] = makeVGAEntry(c, color);
}

void terminalPutChar(char c)
{
	terminalPutEntryAt(c, terminalColor, terminalCol, terminalRow);
	if (terminalCol++ == VGA_WIDTH) {
		terminalCol = 0;
		if (terminalRow++ == VGA_HEIGHT)
			terminalRow = 0;
	}
}

void terminalWriteString(const char *data)
{
	size_t dataLen = strlen(data);
	for (size_t i = 0; i < dataLen; i++)
		terminalPutChar(data[i]);
}

void brados_main()
{
	terminalInit();
	terminalWriteString("Welcome to BRaDOS!\n");
}
