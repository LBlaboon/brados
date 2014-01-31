#ifndef __brados_multiboot_h__
#define __brados_multiboot_h__

#include <stdint.h>

// For more information on this structure, see:
// http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
struct multiboot_header {
	// Indicates the presence and validity of other fields
	uint32_t flags;
	
	// Present if bit 0 in 'flags' is set
	uint32_t mem_lower;
	uint32_t mem_upper;
	
	// Present if bit 1 in 'flags' is set
	uint32_t boot_device;
	
	// Present if bit 2 in 'flags' is set
	uint32_t cmdline;
	
	// Present if bit 3 in 'flags' is set
	uint32_t mods_count;
	uint32_t mods_addr;
	
	// Present if bits 4 or 5 are present in 'flags'
	// NOTE: Bits 4 and 5 are mutually exclusive. Bit 4 indicates fields
	// related to a.out kernel images, while bit 5 indicated fields related
	// to ELF kernel images. Since BRaDOS is an ELF kernel, the bit 5 field
	// names will be used.
	uint32_t shdr_num;	// "tabsize" for an a.out kernel
	uint32_t shdr_size;	// "strsize" for an a.out kernel
	uint32_t shdr_addr;	// "addr" for an a.out kernel
	uint32_t shdr_shndx;	// "reserved" for an a.out kernel
	
	// Present if bit 6 in 'flags' is set
	uint32_t mmap_length;
	uint32_t mmap_addr;
	
	// Present if bit 7 in 'flags' is set
	uint32_t drives_length;
	uint32_t drives_addr;
	
	// Present if bit 8 in 'flags' is set
	uint32_t config_table;
	
	// Present if bit 9 in 'flags' is set
	uint32_t boot_loader_name;
	
	// Present if bit 10 in 'flags' is set
	uint32_t apm_table;
	
	// Present if bit 11 in 'flags' is set
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

struct multiboot_mmap {
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
};

struct multiboot_mod {
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t string;
	uint32_t reserved;
};

uint32_t multiboot_getAddress();
uint32_t multiboot_getMagic();

#endif
