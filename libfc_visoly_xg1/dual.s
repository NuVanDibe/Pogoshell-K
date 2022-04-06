
	.SECTION	.iwram

	.ALIGN
	.ARM

	.GLOBAL		executeCart
	.GLOBAL		setRamStart

Setup_VIS:

	add	r1, pc, #1
	bx		r1

	.THUMB

	nop
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

	.ARM


@ r0 = ROM start
setRomStart_VIS:

	stmfd	sp!, {lr}

	mov		r9,r0

	bl	Setup_VIS

	mov		r0,r9

	@ k = ((l>>12)&0x7F8) | ((l>>22)&0x7) | 0x4000;

	mov		r2, r0, lsr #22
	mov		r0, r0, lsr #12

	mov		r3, #0xff

	and		r2, r2, #7
	and		r0, r0, r3, lsl #3

	orr		r0, r0, r2

	mov		r3, #0x40
	orr		r0, r0, r3, lsl #8

	ldr		r1,=0x096B592E
	strh	r0,[r1]

	ldmfd	sp!,{lr}
	bx		lr


@ r0 = Rom start offset
setRomStart_XG:

	add	r1, pc, #1
	bx		r1

	.THUMB


	lsr		r0,r0,#15
	ldr		r1,=0xFFF
	and		r0,r1

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
	strh	r0,[r4]
	strh	r2,[r6]

	bx		lr

	.ARM

@ r0 = RAM 64KB bank
setRamStart_VIS:

	stmfd	sp!, {lr}

	mov		r9,r0

	bl		Setup_VIS

	ldr		r1,=0x0942468A
	strh	r9,[r1]

	ldmfd	sp!,{lr}
	bx		lr


@ r0 = RAM 64KB bank
setRamStart_XG:

	add		r1, pc, #1
	bx		r1

	.THUMB

	lsl		r0,r0,#2

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
	strh	r0,[r5]
	strh	r2,[r6]

	bx		lr

	.ARM

@ r1 = bank
setRamStart:

	stmfd sp!,{r4-r11,lr}

	mov		r11,r1

	mov		r0,r11
	bl		setRamStart_XG

	mov		r0,r11
	bl		setRamStart_VIS

	ldmfd	sp!,{r4-r11,lr}

	bx		lr

@ r1 = jump adress, r2 = rom start value
executeCart:

	mov		r11,r1
	mov		r12,r2

	mov		r0,r12
	bl		setRomStart_XG

	mov		r0,r12
	bl		setRomStart_VIS

	ldr		r0, =reset_func
	ldr		r1, =reset_end
	mov		r2, #0x03000000
	add		r2, r2, #0x7E00
	mov		r3,#0
	strb	r3,[r2, #0x1FA]
0:
	ldr		r3,[r0],#4
	str		r3,[r2],#4
	cmp		r0,r1
	bne		0b

	mov		r2, #0x03000000
	add		r1, r2, #0x7E00
@	add		r2, r1, #0x200

	bx		r1

reset_func:

@	add		r1,#0x100
@	mov		r3,#0
@0:
@	str		r3,[r1, #4]!
@	cmp		r1,r2
@	bne		0b

@	mov		r0,#0xfc		@reset all execept RAM
	mov		r0,#0xfe
	swi		#0x010000

	cmp		r11,#0
	bne		1f

	swi		#0x000000
1:
	bx		r11
reset_end:

	.ALIGN
ez_constants:
	.word	0x09FE0000
	.word	0x08000000
	.word	0x08020000
	.word	0x08040000
	.word	0x09880000
	.word	0x09C00000
	.word	0x09FC0000


	.ALIGN
	.POOL
	.END
