.section ".text.boot"

.globl Start
Start:
	// Setup the stack
	mov	sp, #0x8000
	
	// Clear out bss
	ldr	r4, =_bss_start
	ldr	r9, =_bss_end
	mov	r5, #0
	mov	r6, #0
	mov	r7, #0
	mov	r8, #0
	b	2f

1:
	// Store multiple at r4
	stmia	r4!, {r5-r8}

2:
	// If we are still below bss_end, loop
	cmp	r4, r9
	blo	1b
	
	// Call brados_main
	ldr	r3, =brados_main
	blx	r3

halt:
	// Halt
	wfe
	b	halt
