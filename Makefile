# Build environment
PREFIX	?= /usr/local/cross
ARCH	?= i586-elf
GNU	?= $(PREFIX)/$(ARCH)/bin/$(ARCH)

# Source files
SOURCES_ASM	:= arch/$(ARCH)/boot.s
SOURCES_C	:= arch/$(ARCH)/multiboot.c driver/video/vga.c kernel/brados.c kernel/printk.c lib/string.c

# Object files
OBJS	:= $(patsubst %.s,%.o,$(SOURCES_ASM))
OBJS	+= $(patsubst %.c,%.o,$(SOURCES_C))

# Build flags
CFLAGS = -std=gnu99 -ffreestanding -Iinclude/ -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib

# Build rules
all: brados.bin
.PHONY: all clean

brados.bin: $(OBJS) arch/$(ARCH)/linker.ld
	$(GNU)-gcc -T arch/$(ARCH)/linker.ld -o $@ $(LDFLAGS) $(OBJS) -lgcc

clean:
	rm -f $(OBJS) brados.bin

# C
%.o: %.c Makefile
	$(GNU)-gcc -c $< -o $@ $(CFLAGS)

# Assembly
%.o: %.s Makefile
	$(GNU)-as $< -o $@
