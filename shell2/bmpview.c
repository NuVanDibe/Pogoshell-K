
#include <pogo.h>
#include "misc.h"
#include "freespace.h"

const
#include "gammatab25.h"

const
#include "gammatab55.h"

const
#include "gammatab75.h"

//static unsigned short bmp_colors[256];
static unsigned char *bmp_ptr;
static int bmp_h;
static int bmp_w;
static int bmp_bpp;
static unsigned char *bmp_cols;
const unsigned char *gammatabs[4] = {0, gammatab75, gammatab55, gammatab25};

#define BG_PALRAM ((uint16*)0x05000000)

void set_gamma(int level)
{
	int i;
	const unsigned char *gamma = gammatabs[level];

	for(i=0; i<256; i++)
	{
		if(level)
			BG_PALRAM[i] = (gamma[bmp_cols[i*4]]<<10) | (gamma[bmp_cols[i*4+1]]<<5) | (gamma[bmp_cols[i*4+2]]);
		else
			BG_PALRAM[i] = ((bmp_cols[i*4]&0xf8)<<7) | ((bmp_cols[i*4+1]&0xf8)<<2) | ((bmp_cols[i*4+2]&0xf8)>>3);
	}
}

void prepare_bmp(unsigned char *ptr, uint16 *colors)
{
	int i,size, sfd;

	size = *((int *)&ptr[14]);
	bmp_w = *((int *)&ptr[18]);
	bmp_h = *((int *)&ptr[22]);
	bmp_bpp = (*((short *)&ptr[28])) / 8;

	bmp_w = (bmp_w+3) & 0xFFFFFFFC;

	sfd = open("/dev/screen", 0);

	if(bmp_bpp == 1)
	{
		bmp_cols = &ptr[size+14];
		for(i=0; i<256; i++)
		{
			BG_PALRAM[i] = ((bmp_cols[i*4]&0xf8)<<7) | ((bmp_cols[i*4+1]&0xf8)<<2) | ((bmp_cols[i*4+2]&0xf8)>>3);
		}
		ioctl(sfd, SC_SETMODE, 1);

	}
	else
		ioctl(sfd, SC_SETMODE, 2);

	bmp_ptr = &ptr[*((int *)&ptr[10])];
	close(sfd);
}

void set_palette(uint16 *colors, int fader)
{
}


#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

void render_bmp(int x, int y, int scalex, int scaley)
{
	int wi;
	unsigned char *src;
	unsigned short *dst, *s16;

	int w = MIN(bmp_w, 240);
	int h = MIN(bmp_h, 160);

	if((x+240) > bmp_w)
		x = (bmp_w-240);
	if((y+160) > bmp_h)
		y = (bmp_h-160);

	if(x < 0) x = 0;
	if(y < 0) y = 0;

	src = &bmp_ptr[(x + (bmp_h - y - 1) * bmp_w) * bmp_bpp];
	dst = (unsigned short *)0x06000000; //vram

/*
	{
		src = bmp_ptr;
		i = (x + (bmp_h - y - 1) * bmp_w)<<8;

		w /= 2;
		while(h--)
		{
			wi = w;
			while(wi--)
			{
				c = src[i>>8]<<8;
				i += stepx;
				c |= src[i>>8];
				i += stepx;
				*dst++ = c;
			}
			dst += (120 - w);
			src -=  bmp_w;
			i -= (stepx*w*2 + stepy*bmp_w);
		}

	}
*/
	switch(bmp_bpp)
	{
	case 1:
		s16 = (unsigned short *)src;
		w /= 2;
		while(h--)
		{
			wi = w;
			while(wi--)
				//*dst++ = bmp_colors[*src++];
				*dst++ = *s16++;
			dst += (120 - w);
			s16 -= (w + bmp_w/2);
		}
		break;
/*	case 2:
		s16 = (unsigned short *)src;
		while(h--)
		{
			wi = w;
			while(wi--)
				*dst++ = *s16++;
			dst += (240 - w);
			s16 -= (w + bmp_w);
		}
		break;*/
	case 3:
		while(h--)
		{
			wi = w;
			while(wi--)
			{
				*dst++ = ((src[0]&0xf8)<<7) | ((src[1]&0xf8)<<2) | ((src[2]&0xf8)>>3);
				src += 3;
			}
			dst += (240 - w);
			src -= (w + bmp_w) * 3;
		}
		break;
	}
}




void bmp_view(char *fname)
{
	int glevel = 0;
	int c,fd, quit = 0;
	int x=0, y=0;
	int dx=0, dy=0;
	int speed = 8;
	unsigned int *p = (unsigned int *)0x06000000;
	int l = 60*160;
	while(l--)
		*p++ = 0;

	uchar *bmp = file2mem(fname, (void *)(FREEPTR), 120*1024);

	prepare_bmp(bmp, NULL);

	while(!quit)
	{
		render_bmp(x, y, 256, 256);

		//while((c = getchar()) == EOF);
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
		case RAWKEY_SELECT:
			if(!(c&0x80))
			{
				glevel = (glevel+1)&0x3;
				set_gamma(glevel);
			}
			break;
		case RAWKEY_A:
		case RAWKEY_B:
			if(c != (RAWKEY_A|0x80))
				quit = 1;
			break;
		}
		x += dx;
		y += dy;
		if((x+240) > bmp_w)
			x = (bmp_w-240);
		if((y+160) > bmp_h)
			y = (bmp_h-160);

		if(x < 0) x = 0;
		if(y < 0) y = 0;
	}
 
	fd = open("/dev/screen", 0);
	ioctl(fd, SC_SETMODE, 2);
	close(fd);
}
