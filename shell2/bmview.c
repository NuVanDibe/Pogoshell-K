
#include <pogo.h>
#include "misc.h"

#define BM 0x4D42
#define BMHEADERSIZE 8

void bm_view(char *fname);

static uint16 *bm_ptr;
static int bm_h;
static int bm_w;
static int bm_size;

int prepare_bm(uint16 *ptr)
{
	int sfd;

	if (!ptr || ptr[0] != BM || ptr[1] != 0x10)
		return 1;

	bm_w = ptr[2];
	bm_h = ptr[3];

	if (bm_size != bm_w * bm_h * 2 + BMHEADERSIZE)
		return 2;

	sfd = open("/dev/screen", 0);
	ioctl(sfd, SC_SETMODE, 2);
	close(sfd);

	bm_ptr = &ptr[4];
	return 0;
}

#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

void render_bm(int x, int y)
{
	int wi;
	uint16 *src, *dst;

	int w = MIN(bm_w, 240);
	int h = MIN(bm_h, 160);

	if((x+240) > bm_w)
		x = (bm_w-240);
	if((y+160) > bm_h)
		y = (bm_h-160);

	if(x < 0) x = 0;
	if(y < 0) y = 0;

	src = &bm_ptr[y*bm_w+x];
	dst = (uint16 *)0x06000000; //vram

	while(h--)
	{
		wi = w;
		while(wi--)
			*dst++ = *src++;
		dst += (240 - w);
	}
}

void bm_view(char *fname)
{
	int c, l, r, fd, quit = 0;
	int x=0, y=0;
	int dx=0, dy=0;
	int speed = 8;
	uint16 *bm;
	uint32 *p = (uint32 *)0x06000000;

	fd = open(fname, 0);
	bm = (uint16 *)lseek(fd, 0, SEEK_MEM);
	bm_size = lseek(fd, 0, SEEK_END);
	close(fd);

	r = prepare_bm(bm);

	if (r) {
		l = 120*160;
		while(l--)
			*p++ = 0x7fff7fff;
	} else {
		l = 120*160;
		while(l--)
			*p++ = 0;
		while(!quit)
		{
			render_bm(x, y);

			c = getchar();

			switch(c&0x7F)
			{
				case RAWKEY_L:
					speed = c&0x80 ? 8 : 128;
					break;
				case RAWKEY_RIGHT:
					dx = c&0x80 ? 0 : speed;
					break;
				case RAWKEY_LEFT:
					dx = c&0x80 ? 0 : -speed;
					break;
				case RAWKEY_UP:
					dy = c&0x80 ? 0 : -speed;
					break;
				case RAWKEY_DOWN:
					dy = c&0x80 ? 0 : speed;
					break;
				case RAWKEY_A:
				case RAWKEY_B:
					if(c != (RAWKEY_A|0x80))
						quit = 1;
					break;
			}
			x += dx;
			y += dy;
			if((x+240) > bm_w)
				x = (bm_w-240);
			if((y+160) > bm_h)
				y = (bm_h-160);

			if(x < 0) x = 0;
			if(y < 0) y = 0;
		}
	}
 
	fd = open("/dev/screen", 0);
	ioctl(fd, SC_SETMODE, 2);
	close(fd);
}
