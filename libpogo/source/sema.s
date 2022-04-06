@ Software semaphores

	.SECTION	.iwram

	.ALIGN
	.ARM

	.GLOBAL		getSema
	.GLOBAL		trySema
	.GLOBAL		relSema
	.GLOBAL		haltCpu
	.GLOBAL		suspendGBA

suspendGBA:
	swi		#0x30000
	bx		lr

haltCpu:
	swi		#0x20000
	bx		lr

getSema:
		mov		r0,r1
		mov		r1, #1
0:
		swp		r2,r1,[r0]
		cmp		r2, r1
		beq		0b
		bx		lr

trySema:
		mov		r0,r1
		mov		r1, #1
		swp		r2,r1,[r0]
		cmp		r2, r1
		beq		0f
		mov		r0, #1
		bx		lr
0:
		mov		r0, #0
		bx		lr


relSema:
		mov		r0,r1
		mov		r1, #0
		str		r1,[r0]

		bx		lr


Reset:
	
