#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <brados/printk.h>
#include <brados/string.h>
#include <video/vga.h>

const char *BASE16 = "0123456789ABCDEF";
const char *BASE16L = "0123456789abcdef";

// Print an unsigned number as a string; return the number of characters printed
static int printNum(struct vgastate *term, uintmax_t num, unsigned int base, const char *digits)
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

// Determines the printed length of a number
static unsigned int numLen(uintmax_t num, unsigned int base)
{
	unsigned int len = 1;
	while (num >= base) {
		len++;
		num /= base;
	}
	
	return len;
}

// Print a formatted string to the given terminal
int printk(struct vgastate *term, const char *fmt, ...)
{
	int numPrinted = 0;
	va_list args;
	va_start(args, fmt);
	
	// The bits of 'mode' explained:
	// 0: Whether we are printing characters normally or parsing the format
	// 1: Whether we are still parsing flags
	// 2: Whether we are looking at width or precision (have we seen a .)
	// 3: + flag
	// 4: space flag
	// 5: - flag
	// 6: # flag
	// 7: 0 flag
	uint8_t mode = 0;
	int length = 0;
	unsigned int width = 0, precision = 0;
	
	for (size_t i = 0; i < strlen(fmt); i++) {
		if (mode % 2 == 0) {
			// Begin parsing format if we reach a '%'
			if (fmt[i] == '%') {
				mode |= 0x01;
			// Or just print the character if we're not parsing
			} else {
				term_putChar(term, fmt[i]);
				numPrinted++;
			}
			
			continue;
		}
		
		switch (fmt[i]) {
		case '%':
			// Print a percent sign
			term_putChar(term, '%');
			numPrinted++;
			mode = 0;
			break;
		case '+':
			// Set the plus flag
			if ((mode >> 1) % 2 == 0)
				mode |= 0x08;
			
			break;
		case ' ':
			// Set the space flag
			if ((mode >> 1) % 2 == 0)
				mode |= 0x10;
			
			break;
		case '-':
			// Set the left-align flag
			if ((mode >> 1) % 2 == 0)
				mode |= 0x20;
			
			break;
		case '#':
			// Set the alternate flag
			if ((mode >> 1) % 2 == 0)
				mode |= 0x40;
			
			break;
		case '0':
			// Set the zero-pad flag
			if ((mode >> 1) % 2 == 0)
				mode |= 0x80;
			// Next digit in width specifier
			else if ((mode >> 2) % 2 == 0)
				width *= 10;
			// Next digit in precision specifier
			else
				precision *= 10;
			
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			// Next digit in width specifier
			if ((mode >> 2) % 2 == 0) {
				mode |= 0x02;
				width *= 10;
				width += (fmt[i] - 48); // 0 = ascii 48
			// Next digit in precision specifier
			} else {
				precision *= 10;
				precision += (fmt[i] - 48);
			}
			
			break;
		case '.':
			// Delimit width and precision
			if ((mode >> 2) % 2 == 0)
				mode |= 0x06;
			
			break;
		case 'h':
			// Denotes a short or char argument
			mode |= 0x06;
			length--;
			break;
		case 'l':
			// Denotes a long or long long argument
			mode |= 0x06;
			length++;
			break;
		case 'z':
			// Denotes a size_t argument
			mode |= 0x06;
			length = 3;
			break;
		case 'j':
			// Denotes a intmax_t argument
			mode |= 0x06;
			length = 4;
			break;
		case 't':
			// Denotes a ptrdiff_t argument
			mode |= 0x06;
			length = 5;
			break;
		// Print a signed integer
		case 'd':
		case 'i': {
			intmax_t num;
			uintmax_t unum;
			
			// Pull the argument from the list
			switch (length) {
				case 1:
					num = va_arg(args, long);
					break;
				case 2:
					num = va_arg(args, long long);
					break;
				case 3:
					num = va_arg(args, size_t);
					break;
				case 4:
					num = va_arg(args, intmax_t);
					break;
				case 5:
					num = va_arg(args, ptrdiff_t);
					break;
				default:
					num = va_arg(args, int);
					break;
			}
			
			// Get unsigned version
			if (num < 0)
				unum = num * -1;
			else
				unum = num;
			
			// Get the length of the printed number
			unsigned int len = numLen(unum, 10);
			
			// Get the length of the printed number with formatting
			unsigned int flen = len;
			if (precision > len)
				flen += (precision - len);
			if (num < 0)
				flen++;
			if (num >= 0 && (mode >> 3) + (mode >> 4) > 0)
				flen++;
			
			// If specified, print preceding space padding
			if ((mode >> 5) % 2 == 0) {
				if (precision > 0 || (mode >> 7) % 2 == 0) {
					for (int j = width - flen; j > 0; j--) {
						term_putChar(term, ' ');
						numPrinted++;
					}
				}
			}
			
			// If specified print the sign
			if (num < 0) {
				term_putChar(term, '-');
				numPrinted++;
			} else if ((mode >> 3) % 2 == 1) {
				term_putChar(term, '+');
				numPrinted++;
			} else if ((mode >> 4) % 2 == 1) {
				term_putChar(term, ' ');
				numPrinted++;
			}
			
			// If specified, print the precision 0s
			if (precision > 0) {
				for (int j = precision - len; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			} else if ((mode >> 7) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			}
			
			// Print the number
			numPrinted += printNum(term, unum, 10, BASE16);
			
			// If specified, print succeeding spaces
			if ((mode >> 5) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			mode = 0;
			break;
		// Print an unsigned integer
		} case 'u': {
			uintmax_t num;
			
			// Pull the argument from the list
			switch (length) {
				case 1:
					num = va_arg(args, unsigned long);
					break;
				case 2:
					num = va_arg(args, unsigned long long);
					break;
				case 3:
					num = va_arg(args, size_t);
					break;
				case 4:
					num = va_arg(args, intmax_t);
					break;
				case 5:
					num = va_arg(args, ptrdiff_t);
					break;
				default:
					num = va_arg(args, unsigned int);
					break;
			}
			
			// Get the length of the printed number
			unsigned int len = numLen(num, 10);
			
			// Get the length of the printed number with formatting
			unsigned int flen = len;
			if (precision > len)
				flen += (precision - len);
			if ((mode >> 3) % 2 == 1 || (mode >> 4) % 2 == 1)
				flen++;
			
			// If specified, print preceding space padding
			if ((mode >> 5) % 2 == 0) {
				if (precision > 0 || (mode >> 7) % 2 == 0) {
					for (int j = width - flen; j > 0; j--) {
						term_putChar(term, ' ');
						numPrinted++;
					}
				}
			}
			
			// If specified print the sign
			if ((mode >> 3) % 2 == 1) {
				term_putChar(term, '+');
				numPrinted++;
			} else if ((mode >> 4) % 2 == 1) {
				term_putChar(term, ' ');
				numPrinted++;
			}
			
			// If specified, print the precision 0s
			if (precision > 0) {
				for (int j = precision - len; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			} else if ((mode >> 7) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			}
			
			// Print the number
			numPrinted += printNum(term, num, 10, BASE16);
			
			// If specified, print succeeding spaces
			if ((mode >> 5) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			mode = 0;
			break;
		// Print an unsigned integer in hexadecimal
		} case 'x':
		case 'X': {
			uintmax_t num;
			
			// Pull the argument from the list
			switch (length) {
				case 1:
					num = va_arg(args, unsigned long);
					break;
				case 2:
					num = va_arg(args, unsigned long long);
					break;
				case 3:
					num = va_arg(args, size_t);
					break;
				case 4:
					num = va_arg(args, intmax_t);
					break;
				case 5:
					num = va_arg(args, ptrdiff_t);
					break;
				default:
					num = va_arg(args, unsigned int);
					break;
			}
			
			// Get the length of the printed number
			unsigned int len = numLen(num, 16);
			
			// Get the length of the printed number with formatting
			unsigned int flen = len;
			if (precision > len)
				flen += (precision - len);
			if ((mode >> 3) % 2 == 1 || (mode >> 4) % 2 == 1)
				flen++;
			if ((mode >> 6) % 2 == 1)
				flen += 2;
			
			// If specified, print preceding space padding
			if ((mode >> 5) % 2 == 0) {
				if (precision > 0 || (mode >> 7) % 2 == 0) {
					for (int j = width - flen; j > 0; j--) {
						term_putChar(term, ' ');
						numPrinted++;
					}
				}
			}
			
			// If specified print the sign
			if ((mode >> 3) % 2 == 1) {
				term_putChar(term, '+');
				numPrinted++;
			} else if ((mode >> 4) % 2 == 1) {
				term_putChar(term, ' ');
				numPrinted++;
			}
			
			// If specified, print the 0x
			if ((mode >> 6) % 2 == 1) {
				term_putChar(term, '0');
				if (fmt[i] == 'x')
					term_putChar(term, 'x');
				else
					term_putChar(term, 'X');
				numPrinted += 2;
			}
			
			// If specified, print the precision 0s
			if (precision > 0) {
				for (int j = precision - len; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			} else if ((mode >> 7) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			}
			
			// Print the number
			if (fmt[i] == 'x')
				numPrinted += printNum(term, num, 16, BASE16L);
			else
				numPrinted += printNum(term, num, 16, BASE16);
			
			// If specified, print succeeding spaces
			if ((mode >> 5) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			mode = 0;
			break;
		// Print an unsigned integer in octal
		} case 'o': {
			uintmax_t num;
			
			// Pull the argument from the list
			switch (length) {
				case 1:
					num = va_arg(args, unsigned long);
					break;
				case 2:
					num = va_arg(args, unsigned long long);
					break;
				case 3:
					num = va_arg(args, size_t);
					break;
				case 4:
					num = va_arg(args, intmax_t);
					break;
				case 5:
					num = va_arg(args, ptrdiff_t);
					break;
				default:
					num = va_arg(args, unsigned int);
					break;
			}
			
			// Get the length of the printed number
			unsigned int len = numLen(num, 8);
			
			// Get the length of the printed number with formatting
			unsigned int flen = len;
			if (precision > len)
				flen += (precision - len);
			if ((mode >> 3) % 2 == 1 || (mode >> 4) % 2 == 1)
				flen++;
			if ((mode >> 6) % 2 == 1)
				flen++;
			
			// If specified, print preceding space padding
			if ((mode >> 5) % 2 == 0) {
				if (precision > 0 || (mode >> 7) % 2 == 0) {
					for (int j = width - flen; j > 0; j--) {
						term_putChar(term, ' ');
						numPrinted++;
					}
				}
			}
			
			// If specified print the sign
			if ((mode >> 3) % 2 == 1) {
				term_putChar(term, '+');
				numPrinted++;
			} else if ((mode >> 4) % 2 == 1) {
				term_putChar(term, ' ');
				numPrinted++;
			}
			
			// If specified, print the 0x
			if ((mode >> 6) % 2 == 1) {
				term_putChar(term, '0');
				numPrinted++;
			}
			
			// If specified, print the precision 0s
			if (precision > 0) {
				for (int j = precision - len; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			} else if ((mode >> 7) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, '0');
					numPrinted++;
				}
			}
			
			// Print the number
			numPrinted += printNum(term, num, 8, BASE16);
			
			// If specified, print succeeding spaces
			if ((mode >> 5) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			mode = 0;
			break;
		// Print a string
		} case 's': {
			char *s = va_arg(args, char *);
			term_writeStr(term, s);
			numPrinted += strlen(s);
			mode = 0;
			break;
		// Print a character
		} case 'c':
			if (mode % 2 == 0)
				term_putChar(term, 'c');
			else
				term_putChar(term, (char) va_arg(args, int));
			
			numPrinted++;
			mode = 0;
			break;
		// Print a pointer
		case 'p': {
			uintmax_t num = va_arg(args, void *);
			
			// Get the length of the printed number
			unsigned int len = numLen(num, 16);
			unsigned int flen = len + 2;
			precision = sizeof(void *) * 2; // A byte is 2 hex chars
			
			// If specified, print preceding space padding
			if ((mode >> 5) % 2 == 0) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			term_putChar(term, '0');
			term_putChar(term, 'x');
			numPrinted += 2;
			
			// If specified, print the precision 0s
			for (int j = precision - len; j > 0; j--) {
				term_putChar(term, '0');
				numPrinted++;
			}
			
			// Print the number
			numPrinted += printNum(term, num, 16, BASE16);
			
			// If specified, print succeeding spaces
			if ((mode >> 5) % 2 == 1) {
				for (int j = width - flen; j > 0; j--) {
					term_putChar(term, ' ');
					numPrinted++;
				}
			}
			
			mode = 0;
			break;
		} default:
			term_putChar(term, fmt[i]);
			numPrinted++;
			mode = 0;
			break;
		}
	}
	
	va_end(args);
	return numPrinted;
}
