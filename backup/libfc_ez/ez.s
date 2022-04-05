
	.SECTION	.iwram

	.ALIGN
	.ARM

	.GLOBAL		executeCart
	.GLOBAL		setRamStart
	.GLOBAL		setRomStart


@ r1 = Adress, r2 = value  Rom start offset
ezWriteReg:

	stmfd sp!,{r3-r9}

	mov		r3,#0xD200
	mov		r4,#0x1500

	mov		r6,#0x08000000
	add		r7,r6,#0x00020000           @ #0x08020000
	add		r8,r7,#0x00020000			@ #0x08040000

	add		r5,r6,#0x02000000
	sub		r5,r5,#0x00020000			@ #0x09FE0000
	sub		r9,r5,#0x00020000           @ #0x09FC0000
	
	strh	r3,[r5]
	strh	r4,[r6]
	strh	r3,[r7]
	strh	r4,[r8]

	strh	r2,[r1]

	strh	r4,[r9]

	ldmfd	sp!,{r3-r9}
	bx		lr

	.ARM


@ r1 = RAM 16KB bank
setRamStart:

	mov		r2,r1
	ldr		r1,=0x09c00000 
	b		ezWriteReg

	.ARM

@ r1 = jump adress, r2 = rom start value
executeCart:

	mov		r11,r1

	ldr		r1,=0x09880000
	bl		ezWriteReg

	ldr		r0, =reset_func
	ldr		r1, =reset_end
	mov		r2, #0x03000000
	add		r2, r2, #0x7E00
	mov		r3,#0
	str		r3,[r2, #0x1FA]
0:
	ldr		r3,[r0],#4
	str		r3,[r2],#4
	cmp		r0,r1
	bne		0b

	mov		r2, #0x03000000
	add		r1, r2, #0x7E00

	bx		r1

reset_func:

	mov		r3,#0
0:
	str		r3,[r1, #-4]!
	cmp		r1,r2
	bne		0b

	mov		r1,#0
	mov		r0,#0xfc		@reset all execept RAM
	swi		#0x010000
	mov		r0,#0
	cmp		r11,#0
	bne		1f

	swi		#0x000000
1:
	bx		r11
reset_end:

	.ALIGN
	.POOL
	.END
