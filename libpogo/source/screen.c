/*
 * Simple Screen Device (screen.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Simple abstraction of the GBA screen.
 * TODO: Implement read() and write()
 *
 **/


#include "core.h"
#include "device.h"
#include "screen.h"
#include "gba_defs.h"

#define VRAM_BASE ((uint16 *)0x06000000)
#define BG_PALRAM ((uint16*)0x05000000)
#define SETW(adr, val) (*((volatile uint16*)adr) = val)
#define SETL(adr, val) (*((volatile uint32*)adr) = val)
#define GETW(adr) (*(volatile uint16 *)adr)

static Device screendev;

static uchar colors[16*3] = { 
		0x00,0x00,0x00,
		0xC0,0x00,0x00,
		0x00,0xC0,0x00,
		0xC0,0xC0,0x00,
		0x00,0x00,0xC0,
		0xC0,0x00,0xC0,
		0x00,0xC0,0xC0,
		0xC0,0xC0,0xC0,

		0x00,0x00,0x00,
		0xFF,0x00,0x00,
		0x00,0xFF,0x00,
		0xFF,0xFF,0x00,
		0x00,0x00,0xFF,
		0xFF,0x00,0xFF,
		0x00,0xFF,0xFF,
		0xFF,0xFF,0xFF,
};

static int screen_setpalette(uchar *col, int start, int n)
{
	int i,j;
	if(!col)
	{
		col = colors;
		start = 0;
		n = 16;
	}

	for(i=start, j=0; i<start+n; i++, j+=3)
		BG_PALRAM[i] = (((col[j+2]>>3)&0x1F)<<10)|(((col[j+1]>>3)&0x1F)<<5)|((col[j]>>3)&0x1F);
	return 0;
}


static int screen_open(const char *name, int flags)
{
	return 0;
}

static int screen_read(int fd, void *dest, int size)
{
	return 0;
}


static int screen_lseek(int fd, int offset, int origin)
{
	return 0x06000000;
}

static int screen_write(int fd, const void *dest, int size)
{
	return 0;
}

static int screen_ioctl(int fd, int req, va_list vl)
{
	int param;
	switch(req)
	{
	case IO_GETPARAM:
		param = va_arg(vl, int);
		switch(param)
		{
		case SCREEN_WIDTH:
			return 240;
		case SCREEN_HEIGHT:
			return 160;
		}
		break;
	case SC_SETPAL:
		screen_setpalette(va_arg(vl, uchar*), va_arg(vl, int), va_arg(vl, int));
		break;
	case SC_SETMODE:
		//vram_start = 0x06000000;
		switch(va_arg(vl, int))
		{
		case 0:
			SETW(REG_DISPCNT, DISP_LCDC_OFF);
			break;
		case 1:
			SETW(REG_DISPCNT, DISP_MODE_4 | DISP_BG2_ON | DISP_OBJ_ON | DISP_OBJ_CHAR_1D_MAP);
			SETW(REG_BG2CNT, 0);
			break;
		case 2:
			SETW(REG_DISPCNT, DISP_MODE_3 | DISP_BG2_ON | DISP_OBJ_ON | DISP_OBJ_CHAR_1D_MAP);
			SETW(REG_BG2CNT, 0);
			break;
		}
		break;
	//case SC_ALLOC:
	//	param = va_arg(vl, int);
	//	break;

	}
	return 0;
}

void screen_init(void)
{
	/* Setup 8bit bitmap display */
	SETW(REG_DISPCNT, DISP_LCDC_OFF);
	
	//screen_setpalette(colors, 0, 16);

	SETW(REG_DISPCNT, DISP_MODE_3 | DISP_BG2_ON | DISP_OBJ_ON | DISP_OBJ_CHAR_1D_MAP);
	SETW(REG_BG2CNT, 0);

	memset(&screendev, 0, sizeof(screendev));
	screendev.open = screen_open;
	screendev.lseek = screen_lseek;
	screendev.read = screen_read;
	screendev.write = screen_write;
	screendev.ioctl = screen_ioctl;
	device_register(&screendev, "/dev/screen", NULL, -1);
}
