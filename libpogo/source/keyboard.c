/*
 * Simple Keyboard Device (keyboard.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Stdin device for directly reading the GBA buttons
 *
 **/

#include "core.h"
#include "device.h"
#include "gba_defs.h"

extern void get_sema(volatile uint32 *sema);
extern void rel_sema(volatile uint32 *sema);
extern int32 try_sema(volatile uint32 *sema);

static volatile uint32 reader_sema = 0;

static volatile int lastkeys = 0;
static char inbuf[256];
static volatile char *inptr = inbuf;

static Device vkeydev;

static int vkey_open(const char *name, int mode)
{
	return 0;
}

static char pressed[10];

int vkey_get_qualifiers(void)
{
	int q = 0;

	if(pressed[RAWKEY_L]) q |= 1;
	if(pressed[RAWKEY_R]) q |= 2;

	return q;
}

static int vkey_read(int fd, char *dest, int count)
{
	int org = count;
	char *s = inbuf;

	if(inptr == inbuf)
		return 0;

	get_sema(&reader_sema);

	/* Copy from buffer to dest, up to count values */
	while(s < inptr && count) {
		pressed[*s & 0x7F] = !(*s & 0x80);
		*dest++ = *s++;
		count--;
	}

	/* Copy unread chars to beginning of buffer */
	dest = inbuf;
	while(s < inptr) {
		*dest++ = *s++;
	}
	inptr = dest;

	rel_sema(&reader_sema);

	return org-count;
}

/* Call every frame */
static volatile void vkey_update(void)
{
	int diff,i;
	int keys;
	
	i = GETW(REG_IF);
	//if(i & V_BLANK_INTR_FLAG)
	{
		keys = GETW(REG_KEYINPUT);

		keys = (~keys) & 0x3FF;

		diff = ((keys ^ lastkeys) /*& (keys)*/);

		if(!(try_sema(&reader_sema)))
		{
			return;
		}

		if(diff & L_BUTTON)
			*inptr++ = (RAWKEY_L | (keys & L_BUTTON ? 0x00 : 0x80));
		if(diff & R_BUTTON)
			*inptr++ = (RAWKEY_R | (keys & R_BUTTON ? 0x00 : 0x80));
		if(diff & START_BUTTON)
			*inptr++ = (RAWKEY_START | (keys & START_BUTTON ? 0x00 : 0x80));
		if(diff & SELECT_BUTTON)
			*inptr++ = (RAWKEY_SELECT | (keys & SELECT_BUTTON ? 0x00 : 0x80));
		if(diff & U_KEY)
			*inptr++ = (RAWKEY_UP | (keys & U_KEY ? 0x00 : 0x80));
		if(diff & D_KEY)
			*inptr++ = (RAWKEY_DOWN | (keys & D_KEY ? 0x00 : 0x80));
		if(diff & R_KEY)
			*inptr++ = (RAWKEY_RIGHT | (keys & R_KEY ? 0x00 : 0x80));
		if(diff & L_KEY)
			*inptr++ = (RAWKEY_LEFT | (keys & L_KEY ? 0x00 : 0x80));
		if(diff & A_BUTTON)
			*inptr++ = (RAWKEY_A | (keys & A_BUTTON ? 0x00 : 0x80));
		if(diff & B_BUTTON)
			*inptr++ = (RAWKEY_B | (keys & B_BUTTON ? 0x00 : 0x80));
		lastkeys = keys;
		
		rel_sema(&reader_sema);
	}
}

void key_init(void)
{
	uint16 l;
	memset(&vkeydev, 0, sizeof(vkeydev));
	vkeydev.open = vkey_open;
	vkeydev.read = (int (*)(int, void *, int))vkey_read;
	device_register(&vkeydev, "/dev/key", vkey_update, 0);

	lastkeys = 0;

	SETW(REG_IME, 1);
	l = GETW(REG_IE);
    SETW(REG_IE, l | V_BLANK_INTR_FLAG);
	SETW(REG_STAT, 0x28);
}
