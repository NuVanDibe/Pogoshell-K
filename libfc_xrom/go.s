
	.SECTION	.iwram

	.ALIGN
	.ARM

	.GLOBAL		executeCart

@ r1 = jump adress, r2 = rom start value
executeCart:

	mov		r11,r1
	
	mov		r0,r2,LSR #17

	mov		r2,#0x09000000
	mov		r3,#0x8000
	
	strh	r3,[r2]
	strh	r0,[r2]

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

	mov		r0,#0xfc		@reset all execept RAM
	swi		#0x010000

	cmp		r11,#0
	bne		1f

	swi		#0x000000
1:
	bx		r11
reset_end:

	.ALIGN
	.POOL
	.END
