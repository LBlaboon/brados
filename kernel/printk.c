#include <stdarg.h>
#include <stddef.h>

#include <brados/printk.h>
#include <brados/string.h>
#include <video/vga.h>

const char *BASE16 = "0123456789ABCDEF";

static int printNum(struct vgastate *term, unsigned int num, unsigned int base)
{
	int numPrinted = 0;
	int digit = num % base;
	if (num >= base)
		numPrinted += printNum(term, num / base, base);
	term_putChar(term, BASE16[digit]);
	numPrinted++;
	return numPrinted;
}

int printk(struct vgastate *term, const char *fmt, ...)
{
	int numPrinted = 0;
	va_list args;
	va_start(args, fmt);
	
	for (size_t i = 0; i < strlen(fmt); i++) {
		if (fmt[i] == '%') {
			int d;
			unsigned int x;
			char *s;
			switch (fmt[i+1]) {
			case '%':
				term_putChar(term, '%');
				numPrinted++;
				break;
			case 'c':
				term_putChar(term, (char) va_arg(args, int));
				numPrinted++;
				break;
			case 'i':
				d = va_arg(args, int);
				if (d < 0) {
					term_putChar(term, '-');
					numPrinted++;
					d *= -1;
				}
				numPrinted += printNum(term, (unsigned int) d, 10);
				break;
			case 's':
				s = va_arg(args, char *);
				term_writeStr(term, s);
				numPrinted += strlen(s);
				break;
			case 'x':
				x = va_arg(args, unsigned int);
				term_writeStr(term, "0x");
				numPrinted += 2;
				numPrinted += printNum(term, x, 16);
				break;
			default:
				term_putChar(term, fmt[i+1]);
				numPrinted++;
				break;
			}
			
			i++;
		} else {
			term_putChar(term, fmt[i]);
			numPrinted++;
		}
	}
	
	va_end(args);
	return numPrinted;
}
