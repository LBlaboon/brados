#include <stddef.h>

#include <brados/string.h>

// Find the length of a string
size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len] != 0)
		len++;
	return len;
}
