
	.SECTION	.iwram

	.ALIGN
	.ARM

	.GLOBAL		executeCart
	.GLOBAL		setRamStart
	.GLOBAL		haltCpu

haltCpu:
	swi		#0x20000
	bx		lr

init_flashcart:
	ldr		r0, =flash_constants
	ldmia	r0,{r2-r5}
	add		r0,r0,#16

@	mov		r2,#0x5354
@	mov		r3,#0x1234
@	mov		r4,#0x5678
@	mov		r5,#0xabcd

@   WriteRepeat (0x987654, 0x5354, 1);
	ldr		r1,[r0]
	strh	r2,[r1]

@   WriteRepeat ( 0x12345, 0x1234, 500);
	ldr		r1,[r0,#4]
	mov		r12,#500
0:
	strh	r3,[r1]
	subs	r12,r12,#1
	bne		0b

@   WriteRepeat ( 0x12345, 0x5354, 1);
	ldr		r1,[r0,#4]
	strh	r2,[r1]
	
@   WriteRepeat ( 0x12345, 0x5678, 500);
	mov		r12,#500
1:
	strh	r4,[r1]
	subs	r12,r12,#1
	bne		1b

@   WriteRepeat (0x987654, 0x5354, 1);
	ldr		r1,[r0]
	strh	r2,[r1]

@   WriteRepeat ( 0x12345, 0x5354, 1);
	ldr		r1,[r0,#4]
	strh	r2,[r1]

@   WriteRepeat (0x765400, 0x5678, 1);
	ldr		r1,[r0,#12]
	strh	r4,[r1]

@   WriteRepeat ( 0x13450, 0x1234, 1);
	ldr		r1,[r0,#16]
	strh	r3,[r1]

@   WriteRepeat ( 0x12345, 0xabcd, 500);
	ldr		r1,[r0,#4]
	mov		r12,#500
2:
	strh	r5,[r1]
	subs	r12,r12,#1
	bne		2b

@   WriteRepeat (0x987654, 0x5354, 1);
	ldr		r1,[r0]
	strh	r2,[r1]

	bx		lr

cart_type:

	mov		r2,#0x08000000
	mov		r3,#0x90
	strh	r3,[r2]

	ldrh	r0,[r2]
	ldrh	r1,[r2,#2]
	mov		r3,#0xff
	strh	r3,[r2]

	bx		lr

@ r0 = New rom start value
@ 10 bits
@ 9-3 = 16Mbit -> 256Kbit
@ 2-0 = 128Mbit -> 32Mbit
@ Only bits 3,1,0 allowed for older carts

set_rom_start:
	ldr		r1,=rom_start
	ldr		r1,[r1]
	strh	r0,[r1]

	bx		lr

@ 2bits, 64KByte resolution start (0,64,128 or 196)
@ Oldest 64Mbit Flash only have 1bit (128KByte sram)

set_ram_start:
	ldr		r1,=rom_start
	ldr		r1,[r1,#4]
	strh	r0,[r1]

	bx		lr


@ r1 = jump adress, r2 = rom start value

executeCart:

	mov		r6,r1
	mov		r7,r2
	mov		r8,r3

	bl		init_flashcart

	mov		r0,r7


	@ k = ((l>>12)&0x7F8) | ((l>>22)&0x7) | 0x4000;


	mov		r2, r0, lsr #22
	mov		r0, r0, lsr #12

	mov		r3, #0xff

	and		r2, r2, #7
	and		r0, r0, r3, lsl #3

	orr		r0, r0, r2

	mov		r3, #0x40
	orr		r0, r0, r3, lsl #8


@	mov		r3,#7
@	lsr		r2, r0, #22
@	lsr		r0, r0, #12
@	and		r2,r3
@	lsl		r3, r3,#8
@	add		r3,#$f8
@	and		r0,r3
@	orr		r0,r2
@	mov		r3,#4
@	lsl		r3,r3,#12
@	orr		r0,r3
@	nop


	bl		set_rom_start


	mov		r13,#0x03000000
	add		r13,r13,#0x7F00

	cmp		r6,#0
	bne		1f

	swi		#1
	swi		#0
1:
	bx		r6

setRamStart:
	stmfd sp!,{r4-r6,lr}

	mov		r6,r1

	bl		init_flashcart

	mov		r0,r6
	bl		set_ram_start

	ldmfd sp!,{r4-r6,lr}

	bx		lr

	.ALIGN

flash_constants:
	.word	0x5354
	.word	0x1234
	.word	0x5678
	.word	0xabcd

	.word	(0x00987654 * 2) + 0x08000000
	.word	(0x00012345 * 2) + 0x08000000
	.word	(0x00007654 * 2) + 0x08000000
	.word	(0x00765400 * 2) + 0x08000000
	.word	(0x00013450 * 2) + 0x08000000
rom_start:
	.word	0x096B592E
	.word	0x0942468A


	.ALIGN
	.POOL
	.END
)
