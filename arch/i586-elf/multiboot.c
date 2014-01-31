#include <stdint.h>

#include <multiboot.h>

uint32_t multiboot_getAddress()
{
	uint32_t address = 0;
	__asm__(
		"movl %%ebx, %0;"
		:"=r"(address)
	);
	return address;
}

uint32_t multiboot_getMagic()
{
	uint32_t magic = 0;
	__asm__(
		"movl %%eax, %0;"
		:"=r"(magic)
	);
	return magic;
}
