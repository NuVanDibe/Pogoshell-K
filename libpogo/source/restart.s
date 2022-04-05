

	.ALIGN
	.ARM

reset:
	b	start
	.byte 1,2,3,4,5,6
	.word 0

start:
	add	r0, pc, #1
	bx		r0

	.THUMB

	@ XG Reset Romstart

	mov		r1,#0xd2
	lsl		r1,r1,#8
	mov		r2,#0x15
	lsl		r2,r2,#8

	add	r3, pc, #ez_constants-.-2

	ldmia	r3!,{r4-r7}

	strh	r1,[r4]
	strh	r2,[r5]
	strh	r1,[r6]
	strh	r2,[r7]

	ldmia	r3!,{r4-r6}

	mov		r0,#0

	strh	r0,[r4]
	strh	r2,[r6]

	@ Visoly Reset Romstart

	add		r0, pc, #flash_constants-.-2
	ldmia	r0!,{r2-r5}

	ldr		r1,[r0]
	strh	r2,[r1]

	ldr		r1,[r0,#4]
	mov		r7,#250
	add		r7,r7
	mov		r6,r7
0:
	strh	r3,[r1]
	add		r6,r6,#-1
	bne		0b

	ldr		r1,[r0,#4]
	strh	r2,[r1]

	mov		r6,r7
1:
	strh	r4,[r1]
	add		r6,r6,#-1
	bne		1b

	ldr		r1,[r0]
	strh	r2,[r1]

	ldr		r1,[r0,#4]
	strh	r2,[r1]

	ldr		r1,[r0,#12]
	strh	r4,[r1]

	ldr		r1,[r0,#16]
	strh	r3,[r1]

	ldr		r1,[r0,#4]
	mov		r6,r7
2:
	strh	r5,[r1]
	add		r6,r6,#-1
	bne		2b

	ldr		r1,[r0]
	strh	r2,[r1]

	add		r1, pc, #rom_start-.-2
	mov		r0,#0
	ldr		r1,[r1]
	strh	r0,[r1]

	mov		r0,#0
	mov		r1,#0

	swi		1
	swi		0
	nop
	nop

	nop


ez_constants:
	.word	0x09FE0000
	.word	0x08000000
	.word	0x08020000
	.word	0x08040000
	.word	0x09880000
	.word	0x09C00000
	.word	0x09FC0000

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
