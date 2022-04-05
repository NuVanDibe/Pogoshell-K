/*
 * Cursor sprite functions (cursor.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 *
 **/

#include "core.h"
#include "gba_defs.h"
#include "device.h"

int cursor_x = 0;
int cursor_y = 0;
int cursor_offset = 0;

static int show = 0;

void cursor_update(void)
{
	if(show) {
		//cursor_offset = ioctl(0, KC_GETSCREENOFFSET);
		OAM_BASE[0] = OAM_COLOR_256 | (cursor_y  + cursor_offset);
		OAM_BASE[1] = cursor_x;
	} else {
		OAM_BASE[0] = OAM_COLOR_256 | 160;
	}
}

void cursor_set(int x, int y)
{
	if(x != -1)
		cursor_x = x;
	if(y != -1)
		cursor_y = y;
	cursor_update();
}

void cursor_show(int s)
{
	show = s;
	cursor_update();
}

void cursor_init(void)
{
	uint16 *dp;
	int k,l;
	/* Copy pointer to vram */
	dp = OBJ_VRAM;
	k = 1;
	while(k--) {
		l = 8;
		while(l--) {
			if(l > 1)
			{
				*dp++ = 0x0;
				*dp++ = 0x0;
				*dp++ = 0x0;
				*dp++ = 0x0;
			}
			else
			{
				*dp++ = 0x0101;
				*dp++ = 0x0101;
				*dp++ = 0x0101;
				*dp++ = 0x0101;
			}
		}
	}
	OAM_BASE[0] = 0x0400 | OAM_COLOR_256 | 160;
	OAM_BASE[1] = 0;
	OAM_BASE[2] = 0x0200;
	OAM_BASE[3] = 0;
	
	dp = 0x05000200;
	*dp++ = 0x001F;
	*dp++ = 0x001F;
	*dp++ = 0x001F;
	*dp++ = 0x001F;

	cursor_x = 0;
	cursor_y = 0;
	cursor_update();
}
