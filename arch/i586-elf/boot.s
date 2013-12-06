# Declare constants for creating a multiboot header
.set ALIGN,	1<<0
.set MEMINFO,	1<<1
.set FLAGS,	ALIGN | MEMINFO
.set MAGIC,	0x1BADB002
.set CHECKSUM,	-(MAGIC + FLAGS)

# Declare multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Create a 16 KiB stack
.section .bootstrap_stack
stack_bottom:
.skip 16384
stack_top:

# Kernel entry point
.section .text
.global _start
.type _start, @function
_start:
	# We are now in kernel mode!
	# Setup the stack
	movl $stack_top, %esp
	
	# Call the main kernel function
	call brados_main
	
	# In case the function returns, enter an endless loop
	cli
	hlt
.Lhang:
	jmp .Lhang

# Set the size of the _start symbol for debugging
.size _start, . - _start
