#include <stdarg.h>
#include <stddef.h>

#include <brados/printk.h>
#include <brados/string.h>
#include <video/vga.h>

const char *BASE16 = "0123456789ABCDEF";
const char *BASE16L = "0123456789abcdef";

// Print an unsigned number as a string; return the number of characters printed
static int printNum(struct vgastate *term, unsigned long long num, unsigned int base, const char *digits)
{
	if (base > strlen(digits))
		return 0;
	
	int numPrinted = 0;
	int digit = num % base;
	if (num >= base)
		numPrinted += printNum(term, num / base, base, digits);
	term_putChar(term, digits[digit]);
	numPrinted++;
	return numPrinted;
}

// Print a formatted string to the given terminal
int printk(struct vgastate *term, const char *fmt, ...)
{
	int numPrinted = 0;
	va_list args;
	va_start(args, fmt);
	
	size_t i = 0;
	while (i < strlen(fmt)) {
		if (fmt[i] == '%') {
			i++;
			int end = 0, lng = 0;
			
			while (end == 0) {
				switch (fmt[i]) {
				// Print a percent sign
				case '%':
					term_putChar(term, '%');
					numPrinted++;
					end = 1;
					break;
				// Print a single character
				case 'c':
					term_putChar(term, (char) va_arg(args, int));
					numPrinted++;
					end = 1;
					break;
				// Print an integer number
				case 'd':
				case 'i':
					switch (lng) {
					case 0: {
						int d;
						d = va_arg(args, int);
						if (d < 0) {
							term_putChar(term, '-');
							numPrinted++;
							d *= -1;
						}
						numPrinted += printNum(term, (unsigned int) d, 10, BASE16);
						break;
					} case 1: {
						long ld;
						ld = va_arg(args, long);
						if (ld < 0) {
							term_putChar(term, '-');
							numPrinted++;
							ld *= -1;
						}
						numPrinted += printNum(term, (unsigned long) ld, 10, BASE16);
						break;
					} default: {
						long long lld;
						lld = va_arg(args, long long);
						if (lld < 0) {
							term_putChar(term, '-');
							numPrinted++;
							lld *= -1;
						}
						numPrinted += printNum(term, (unsigned long long) lld, 10, BASE16);
						break;
					}}
					end = 1;
					break;
				// Expect a long (long) value
				case 'l':
					lng++;
					break;
				// Store the number of printed characters
				case 'n': {
					int *n;
					n = va_arg(args, int *);
					*n = numPrinted;
					end = 1;
					break;
				// Print an unsigned integer in octal form
				} case 'o':
					term_putChar(term, '0');
					numPrinted++;
					switch (lng) {
					case 0: {
						unsigned int o;
						o = va_arg(args, unsigned int);
						numPrinted += printNum(term, o, 8, BASE16);
						break;
					} case 1: {
						unsigned long lo;
						lo = va_arg(args, unsigned long);
						numPrinted += printNum(term, lo, 8, BASE16);
						break;
					} default: {
						unsigned long long llo;
						llo = va_arg(args, unsigned long long);
						numPrinted += printNum(term, llo, 8, BASE16);
						break;
					}}
					end = 1;
					break;
				// Print a null-terminated string
				case 's': {
					char *s;
					s = va_arg(args, char *);
					term_writeStr(term, s);
					numPrinted += strlen(s);
					end = 1;
					break;
				// Print an unsigned integer value
				} case 'u':
					switch (lng) {
					case 0: {
						unsigned int u;
						u = va_arg(args, unsigned int);
						numPrinted += printNum(term, u, 10, BASE16);
						break;
					} case 1: {
						unsigned long lu;
						lu = va_arg(args, unsigned long);
						numPrinted += printNum(term, lu, 10, BASE16);
						break;
					} default: {
						unsigned long long llu;
						llu = va_arg(args, unsigned long long);
						numPrinted += printNum(term, llu, 10, BASE16);
						break;
					}}
					end = 1;
					break;
				// Print an unsigned integer in lower hex form
				case 'x':
					term_writeStr(term, "0x");
					numPrinted += 2;
					switch (lng) {
					case 0: {
						unsigned int x;
						x = va_arg(args, unsigned int);
						numPrinted += printNum(term, x, 16, BASE16L);
						break;
					} case 1: {
						unsigned long lx;
						lx = va_arg(args, unsigned long);
						numPrinted += printNum(term, lx, 16, BASE16L);
						break;
					} default: {
						unsigned long long llx;
						llx = va_arg(args, unsigned long long);
						numPrinted += printNum(term, llx, 16, BASE16L);
						break;
					}}
					end = 1;
					break;
				// Print an unsigned integer in upper hex form
				case 'X':
					term_writeStr(term, "0X");
					numPrinted += 2;
					switch (lng) {
					case 0: {
						unsigned int x;
						x = va_arg(args, unsigned int);
						numPrinted += printNum(term, x, 16, BASE16);
						break;
					} case 1: {
						unsigned long lx;
						lx = va_arg(args, unsigned long);
						numPrinted += printNum(term, lx, 16, BASE16);
						break;
					} default: {
						unsigned long long llx;
						llx = va_arg(args, unsigned long long);
						numPrinted += printNum(term, llx, 16, BASE16);
						break;
					}}
					end = 1;
					break;
				// Exit if we reach the end of the string
				case '\0':
					end = 1;
					break;
				// Unknown character; just print the character
				default:
					term_putChar(term, fmt[i]);
					numPrinted++;
					end = 1;
					break;
				}
				i++;
			}
		} else {
			term_putChar(term, fmt[i]);
			numPrinted++;
			i++;
		}
	}
	
	va_end(args);
	return numPrinted;
}
