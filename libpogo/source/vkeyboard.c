/*
 * Virtual Keyboard Device (vkeyboard.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Implements a virtual keyboard that lets the user enter keys into
 * stdin by using combinations of L+R to select a group and joypad,
 * A and B to select a key in that group.
 *
 **/

#include "core.h"
#include "device.h"
#include "gba_defs.h"

extern void get_sema(volatile uint32 *sema);
extern void rel_sema(volatile uint32 *sema);
extern int32 try_sema(volatile uint32 *sema);

static volatile uint32 reader_sema;
static volatile int lastkeys ; //= 0;

static int merge_keys; // = 0;
static int keybox; // = 0;

/* The 5 groups of keys that can be selected with L+R combinations */
static const char *keyset[5] = {
	"rtyfhvbng ",
	"uiojlm,.kp",
	"qweadzxcs\\",
	//"-=B[E;'#]/",
	"pBC;#,. 'E",
	"7894612350"
};

static volatile int vkey_mode; // = 0;

static int cursor_offset;

/*

Meaning of uppercase letters in the key_mapping:

T = Tab
B,C = Backspace
E = Enter
I = Insert
H = sHift
^ = undefined
U = Up
L = Left
R = Right
D = Down
*/


static const int keymap_width = 36;
static const int keymap_height = 6;

/* This describes the layout of the keyboard. Each char represents a 4x4 pixel area */
static const char key_mapping[] = {
"qqwweerrttyyuuiiooppBBCCEEE^^^778899"
"qqwweerrttyyuuiiooppBBCCEEE^^^778899"
"^aassddffgghhjjkkll;;''##EE^^^445566"
"^aassddffgghhjjkkll;;''##EE^^^445566"
"\\\\zzxxccvvbbnnmm,,..       ^00112233"
"\\\\zzxxccvvbbnnmm,,..       ^00112233"
};



static char inbuf[256];

static volatile char *inptr;

static Device vkeydev;

/* Marks all keys in the string 'which' on the keyboard */
static void mark_keys(const char *which)
{
	int i,k,x,y;
	int runs = 10;
	k = 0;

	while(runs) {
		for(i=0, y=0; y<keymap_height; y++)
			for(x=0; x<keymap_width; x++, i++) {
				if(key_mapping[i] == which[k]) {
					//OAM_BASE[4+k*4] = OAM_COLOR_256 | 86+y*5;
					//OAM_BASE[5+k*4] = 0x4000 | 6+x*5;
					OAM_BASE[4+k*4] = OAM_COLOR_256 | (130+y*5);
					OAM_BASE[5+k*4] = 0x4000 | (1+x*5);
					k++;
					if(!which[k])
						return;
				}

			}
		runs--;
	}
}

static int keyboard_height = 32;

/* Sets the keyboard bitmap to use */
static void vkey_setkeyimage(uint16 *data)
{
	int i;
	uint16 *vram = (uint16 *)(0x0600A000 + 240*(160-keyboard_height));
	uint16 count = *data++;

	if(count > 240)
		return;

	for(i=256-count; i<256; i++)
		BG_PALRAM[i] = *data++;

	count = (256-count) | ((256-count)<<8);
	for(i=0; i<120*keyboard_height; i++) {
		*vram++ = (*data++) + count;
	}
}

/* Setup the transparent square sprites to use for marking keys */
static void set_sprites(void)
{
	uint16 *dp;
	int l,i,j,k;
	uint32 colors[] = { 0x00FF8800, 0xFFFFFFFF, 0x00808080, 0x00000080, 0x00800000 };
	uchar *col = (uchar *)colors;

	/* Clear objects to stripes (debug) */
	dp = OBJ_VRAM;
	k = 8192;
	while(k--)
		*dp++ = 0x0101;

	/* Skip first sprite (reserved for console cursor) */
	dp = OBJ_VRAM + 32;

	// Make blend keys
	l = 0x0303;
	for(i=0; i<3; i++, l += 0x0101) {
		k = 8*4;
		while(k--)
			*dp++ = l;

		k = 8;
		while(k--) {
			*dp++ = l;
			*dp++ = 0;
			*dp++ = 0;
			*dp++ = 0;
		}
		k = 2*4;
		while(k--)
			*dp++ = l;
		k = 6*4;
		while(k--)
			*dp++ = 0;
		k = 2;
		while(k--) {
			*dp++ = l;
			*dp++ = 0;
			*dp++ = 0;
			*dp++ = 0;
		}
		k = 6*4;
		while(k--)
			*dp++ = 0;
	}
	
	/* Turn off all sprites */
	for(i=0; i<128; i++) {
		OAM_BASE[i*4] = OAM_OBJ_OFF;
	}

	for(i=0; i<8; i++) {
		OAM_BASE[4+i*4] = OAM_COLOR_256 | 160;
		OAM_BASE[5+i*4] = 0x4000;
		OAM_BASE[6+i*4] = 514;
		OAM_BASE[7+i*4] = 0;
	}

	OAM_BASE[36] = OAM_COLOR_256 | 160;
	OAM_BASE[37] = 0x4000;
	OAM_BASE[38] = 522;
	OAM_BASE[39] = 0;

	OAM_BASE[40] = OAM_COLOR_256 | 160;
	OAM_BASE[41] = 0x4000;
	OAM_BASE[42] = 530;
	OAM_BASE[43] = 0;

	for(i=0; i<256; i++)
		OBJ_PALRAM[i] = (((0x80>>3)&0x1F)<<10)|(((i>>3)&0x1F)<<5)|((0>>3)&0x1F);

	for(i=1, j=0; i<6; i++, j+=4)
		OBJ_PALRAM[i] = (((col[j]>>3)&0x1F)<<10)|(((col[j+1]>>3)&0x1F)<<5)|((col[j+2]>>3)&0x1F);

	SETW(REG_BLDCNT, BLD_BG2_2ND | BLD_OBJ_2ND | BLD_A_BLEND_MODE | BLD_BG2_1ST | BLD_OBJ_1ST );
	SETW(REG_BLDALPHA, 0x0808);
}


static int vkey_open(const char *name, int mode)
{
	return 0;
}

static int vkread(int fd, char *dest, int count)
{
	int org = count;
	char *s = inbuf;

	if(inptr == inbuf)
		return 0;

	get_sema(&reader_sema);

	/* Copy from buffer to dest, up to count values */
	while(s < inptr && count) {
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

static int vkey_read(int fd, char *dest, int count)
{
	int rc, oc = count;
	
	do
	{
		rc = vkread(fd, dest, count);
		if(vkey_mode & KM_ECHO)
			write(1, dest, rc);
		dest += rc;
		count -= rc;
	}
	while((vkey_mode & KM_BLOCKED) && count > 0);
		//Halt();
	return oc-count;
}

void Reset(void);

static int keys_to_boxpos(int keys)
{
	switch(keys)
	{
	case (U_KEY|L_KEY): return 0;
	case U_KEY: return 1;
	case (U_KEY|R_KEY): return 2;
	case L_KEY: return 3;
	case R_KEY: return 4;
	case (D_KEY|L_KEY): return 5;
	case D_KEY: return 6;
	case (D_KEY|R_KEY): return 7;
	case A_BUTTON: return 8;
	case B_BUTTON: return 9;
	default: return -1;
	}
}

static void vkey_update(void) __attribute__ ((noinline));

/* Call every frame */
static void vkey_update(void)
{
	int diff,i;
	int keys,newbox;
	volatile char *op = inptr;
	
	i = GETW(REG_IF);

	if(i & V_COUNT_INTR_FLAG)
	{
		if(vkey_mode & KM_SHOWKBD)
		{
			while(GETW(REG_STAT) & 0x2);
			while(!(GETW(REG_STAT) & 0x2));
			SETL(REG_BG2Y, (160-keyboard_height)<<8);
			SETW(REG_DISPCNT, DISP_MODE_3 | DISP_BG2_ON | DISP_OBJ_ON | DISP_OBJ_CHAR_1D_MAP | DISP_BMP_FRAME_NO);
		}
		return;
	}

	if(i & V_BLANK_INTR_FLAG)
	{
		SETL(REG_BG2Y, 0);
		SETW(REG_DISPCNT, DISP_MODE_3 | DISP_BG2_ON | DISP_OBJ_ON | DISP_OBJ_CHAR_1D_MAP | 0);
		
		if(vkey_mode & KM_SHOWKBD)
			SETL(REG_BG2Y, (keyboard_height)<<8);

		keys = GETW(REG_KEYINPUT);

		keys = (~keys) & 0x3FF;

		if((keys & (START_BUTTON|SELECT_BUTTON)) == (START_BUTTON|SELECT_BUTTON))
			Reset();

		if(!(try_sema(&reader_sema)))
		{
			return;
		}

		if(vkey_mode & KM_RAW)
		{
			diff = ((keys ^ lastkeys) /*& (keys)*/);

			if(diff & L_BUTTON)
				*op++ = (RAWKEY_L | (keys & L_BUTTON ? 0x00 : 0x80));
			if(diff & R_BUTTON)
				*op++ = (RAWKEY_R | (keys & R_BUTTON ? 0x00 : 0x80));
			if(diff & START_BUTTON)
				*op++ = (RAWKEY_START | (keys & START_BUTTON ? 0x00 : 0x80));
			if(diff & SELECT_BUTTON)
				*op++ = (RAWKEY_SELECT | (keys & SELECT_BUTTON ? 0x00 : 0x80));
			if(diff & U_KEY)
				*op++ = (RAWKEY_UP | (keys & U_KEY ? 0x00 : 0x80));
			if(diff & D_KEY)
				*op++ = (RAWKEY_DOWN | (keys & D_KEY ? 0x00 : 0x80));
			if(diff & R_KEY)
				*op++ = (RAWKEY_RIGHT | (keys & R_KEY ? 0x00 : 0x80));
			if(diff & L_KEY)
				*op++ = (RAWKEY_LEFT | (keys & L_KEY ? 0x00 : 0x80));
			if(diff & A_BUTTON)
				*op++ = (RAWKEY_A | (keys & A_BUTTON ? 0x00 : 0x80));
			if(diff & B_BUTTON)
				*op++ = (RAWKEY_B | (keys & B_BUTTON ? 0x00 : 0x80));
			lastkeys = keys;
			inptr = op;
			rel_sema(&reader_sema);
			return;
		}

		diff = ((keys ^ lastkeys) & (keys));
		
		if(diff & SELECT_BUTTON) 
		{
			if(keys & R_BUTTON)
			{
				*op++ = 9;
			}
			else
			if(keys & L_BUTTON)
			{
				*op++ = 27;
			}
			else
			{
				if(vkey_mode & KM_SHOWKBD)
					vkey_mode &= (~KM_SHOWKBD);
				else
					vkey_mode |= KM_SHOWKBD;

				if(vkey_mode & KM_SHOWKBD)
				{
					mark_keys(keyset[keybox]);
					cursor_offset = -keyboard_height;
					//update_cursor();
				} 
				else 
				{
					for(i=0; i<10; i++)
						OAM_BASE[4+i*4] = OAM_COLOR_256 | 160;
					cursor_offset = 0;
					//update_cursor();
				}
			}
		}

		if(diff & START_BUTTON) {
			if(keys & R_BUTTON)
				*op++ = 8;
			else
			if(keys & L_BUTTON)
				*op++ = ' ';
			else
				*op++ = 10;
		}

		newbox = 0;
		if(keys & R_BUTTON)
			newbox =1;
		if(keys & L_BUTTON)
			newbox += 2;
		if(newbox == 3 && (keybox == 2 || keybox == 4))
			newbox++;

		if(keybox != newbox)
		{
			keybox = newbox;
			if(vkey_mode & KM_SHOWKBD)
				mark_keys(keyset[newbox]);
		}

		lastkeys = keys;

		keys &= 0x0F3;
		diff &= 0x0F3;
		
		if(keys)
		{
			merge_keys |= keys;
			i = keys_to_boxpos(merge_keys);
			if(i != -1)
			{
				//OAM_BASE[4+i*4] = OAM_COLOR_256 | 86+y*5;
				//OAM_BASE[5+i*4] = 0x4000 | 6+x*5;
			}
		}
		else
		{
			i = keys_to_boxpos(merge_keys);
			if(i != -1)
			{
				*op++ = keyset[newbox][i];
				if(op[-1] == 'E') op[-1] = 10;
				if(op[-1] == 'B' || op[-1] == 'C') op[-1] = 8;
			}
			merge_keys = 0;
		}
	}
	inptr = op;

	rel_sema(&reader_sema);
}



static int vkey_ioctl(int fd, int req, va_list vl)
{
	int i;
	switch(req) {
	case IO_SETMODE:
		vkey_mode = va_arg(vl, int);
		if(vkey_mode & KM_SHOWKBD)
		{
			mark_keys(keyset[keybox]);
			cursor_offset = -keyboard_height;
			//update_cursor();
		} 
		else 
		{
			for(i=0; i<10; i++)
				OAM_BASE[4+i*4] = OAM_COLOR_256 | 160;
			cursor_offset = 0;
			//update_cursor();		
		}
		return 1;
	case IO_GETMODE:
		return vkey_mode;
	case KC_SETIMAGE:
		vkey_setkeyimage(va_arg(vl, uint16*));
		return 1;
	case KC_GETSCREENOFFSET:
		return cursor_offset;
	}
	return -1;
}

void vkey_init(void)
{
	uint16 l;

	memset(&vkeydev, 0, sizeof(vkeydev));
	vkeydev.open = vkey_open;
	vkeydev.ioctl = vkey_ioctl;
	vkeydev.read = (int (*)(int, void *, int))vkey_read;
	device_register(&vkeydev, "/dev/vkey", vkey_update, 0);

	lastkeys = 0;
	reader_sema = 0;
	inptr = inbuf;
	set_sprites();
	keybox = 0;

	SETW(REG_IME, 1);
	l = GETW(REG_IE);
    SETW(REG_IE, l | V_BLANK_INTR_FLAG | V_COUNT_INTR_FLAG );
	SETW(REG_STAT, 0x28 | ((160-keyboard_height)<<8));


//	dprint("Setup IRQ\n");

}
