	.ALIGN
	.ARM

	.GLOBAL		LZ77UnCompWram
	.GLOBAL		SoftReset

@extern void LZ77UnCompWram(void *Srcp, void *Destp)
LZ77UnCompWram:

	stmfd	sp!,{r0-r12,lr}
0:
	ldrb	r3,[r0],#1
	cmp		r3,#0x20
	beq		0b

	sub		r0,r0,#1

	swi	#0x110000
	ldmfd	sp!,{r0-r12,lr}
	bx lr

	.ALIGN
	.POOL

@extern void SoftReset(unsigned char mask)
SoftReset:

	stmfd	sp!,{r0-r12,lr}

	swi	#0x010000
	ldmfd	sp!,{r0-r12,lr}
	bx lr

	.ALIGN
	.POOL
	.END


