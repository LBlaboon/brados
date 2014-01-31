#include <stddef.h>

#include <brados/string.h>

// Find the first instance of c in str
// Returns -1 if c is not found
int strindexof(char c, const char *str)
{
	for (size_t i = 0; i < strlen(str); i++) {
		if (str[i] == c)
			return i;
	}
	
	return -1;
}

// Find the length of a string
size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len] != 0)
		len++;
	return len;
}
