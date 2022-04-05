
#include <pogo.h>
#include "misc.h"
#include "gba-jpeg-decode.h"
#include "aes.h"

static JPEG_OUTPUT_TYPE *jpg_ptr;
static int jpg_w;
static int jpg_h;

void joint_view(uchar *jpg, int jpg_ram_usage);

#define BG_PALRAM ((uint16*)0x05000000)

int prepare_jpg(unsigned char *ptr, int jpg_ram_usage)
{
	int sfd;

	sfd = open("/dev/screen", 0);
	ioctl(sfd, SC_SETMODE, 2);
	close(sfd);
	
    return JPEG_DecompressImage(ptr, &jpg_ptr, &jpg_w, &jpg_h, (jpg_ram_usage+3)&0xfffffffc);
}

#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

/* By this point, JPEG_DecompressImage isn't ever called again, so
 * it's safe to use at least some of this space.*/
unsigned short *red = (unsigned short *)(0x02000000);
unsigned short *green = (unsigned short *)(0x02000000+(240*sizeof(short)));
unsigned short *blue = (unsigned short *)(0x02000000+((240+240)*sizeof(short)));

#define R(x) (x&31)
#define G(x) ((x>>5)&31)
#define B(x) ((x>>10)&31)

void render_jpg(int x, int y, int scale)
{
	int wi, hi;
	unsigned short *src, *dst;

	int w, h;
	int dh, dw;

	dst = (unsigned short *)0x06000000; //vram

	switch (scale)
	{
		// Aspect
		case 1:
			w = 240;
			h = 240 * jpg_h / jpg_w;
			if (h > 160) {
				h = 160;
				w = 160 * jpg_w / jpg_h;
			}
			dh = 0;
			dst += (160-h)/2*240;
			src = jpg_ptr;
			hi = h*2;
			while (hi--)
			{
				dw = 0;
				wi = w*2;
				if (!(hi&1))
					dst += (240 - w)/2;
				x = 0;
				while (wi--)
				{
					if (!(wi&1) || !(hi&1)) {
						red[x] += R(*src);
						green[x] += G(*src);
						blue[x] += B(*src);
					} else {
						red[x] = R(*src);
						green[x] = G(*src);
						blue[x] = B(*src);
					}
					
					if (!(wi&1) && !(hi&1)) {
						*dst++ = ((red[x]>>2)&31) |
						          (((green[x]>>2)&31)<<5) |
						          (((blue[x]>>2)&31)<<10);
					}

					if (!(wi&1))
						x++;

					dw += jpg_w;
					while (dw >= w*2)
					{
						dw -= w*2;
						src++;
					}
				}
				if (!(hi&1))
					dst += 240-w-(240 - w)/2;
				src -= jpg_w;
				dh += jpg_h;
				while (dh >= h*2)
				{
					dh -= h*2;
					src += jpg_w;
				}
			}
			break;
		// Stretch
		case 2:
			h = 320;
			dh = 0;
			src = jpg_ptr;
			while (h--)
			{
				dw = 0;
				w = 480;
				x = 0;
				while (w--)
				{
					if (!(w&1) || !(h&1)) {
						red[x] += R(*src);
						green[x] += G(*src);
						blue[x] += B(*src);
					} else {
						red[x] = R(*src);
						green[x] = G(*src);
						blue[x] = B(*src);
					}
					
					if (!(w&1) && !(h&1)) {
						*dst++ = ((red[x]>>2)&31) |
						          (((green[x]>>2)&31)<<5) |
						          (((blue[x]>>2)&31)<<10);
					}

					if (!(w&1))
						x++;

					dw += jpg_w;
					while (dw >= 480)
					{
						dw -= 480;
						src++;
					}
				}
				src -= jpg_w;
				dh += jpg_h;
				while (dh >= 320)
				{
					dh -= 320;
					src += jpg_w;
				}
			}
			break;
		default:
			w = MIN(jpg_w, 240);
			h = MIN(jpg_h, 160);

			if((x+240) > jpg_w)
				x = (jpg_w-240);
			if((y+160) > jpg_h)
				y = (jpg_h-160);
			if(x < 0) x = 0;
			if(y < 0) y = 0;

			src = &jpg_ptr[x + y * jpg_w];
			if (h < 160)
				dst += (160 - h)/2*240;
			while(h--)
			{
				wi = w;
				dst += (240 - w)/2;
				while(wi--)
					*dst++ = *src++;
				dst += 240-w-(240 - w)/2;
				src += (jpg_w - w);
			}
			break;
	}
}

void generic_image(void)
{
	int x, y, i1, i2;
	unsigned short *p = (unsigned short *)0x06000000, color;

	i1 = 0;
	i2 = 159*240;
	// Pretty, generic image
    for (y = 0; y < 80; y++)
	{
		for (x = 0; x < 120; x++)
		{
			color = (x*y)/300 + ((119-x)*(79-y)/300);
			//color = (x*y)/300;
			color = color | (color<<5) | (color<<10);
			p[i1+x] = color;
			p[i1+(239-x)] = color;
			p[i2+x] = color;
			p[i2+(239-x)] = color;
		}
		i1 += 240;
		i2 -= 240;
	}
}

void draw_block(int x, int y)
{
	unsigned short *dest = ((unsigned short *)0x06000000);
	unsigned short c;
    int w, h;

	c = 31<<(5*((x>>3)&3));

	for (h = 0; h < 8; h++)
	    for (w = 0; w < 8; w++)
			dest[(y+h)*240+x+w] = c;
}

int decrypt_image(char *fname, uchar *jpg, int msize)
{
	int c, done, jpg_size, x, y;
    uint64 key[2], old_enc[2], *lljpg, *lljpg2, i;
	aes_context ctx;
    
	jpg_size = file2ram(fname, jpg, msize);

    if (jpg_size > sizeof(uint64)*2) {
	    lljpg = (uint64 *) &jpg[sizeof(int)];

    	key[0] = lljpg[0];
    	key[1] = lljpg[1];

		x = y = 0;
		done = 0;
		while (!done)
		{
			while ((c = getchar()) == EOF);
			switch(c)
			{
				case RAWKEY_UP:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					break;
				case RAWKEY_DOWN:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 1;
					break;
				case RAWKEY_LEFT:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 2;
					break;
				case RAWKEY_RIGHT:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 3;
					break;
				case RAWKEY_L:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 4;
					break;
				case RAWKEY_R:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 5;
					break;
				case RAWKEY_A:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 6;
					break;
				case RAWKEY_B:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 7;
					break;
				case RAWKEY_SELECT:
				case RAWKEY_START:
					done = 1;
					break;
			}
			if (x == 240) {
				x = 0;
				y += 8;
			}
			if (y == 160) {
				y = 0;
			}
		}
		aes_set_key(&ctx, (unsigned char *) key, 16, 0);
		old_enc[0] = old_enc[1] = 0xfedcba9876543210ll;
		lljpg2 = (uint64 *) (((unsigned char *) &lljpg[-2]) - sizeof(int));
    	for (i = 2; i < (jpg_size-sizeof(int))/sizeof(uint64); i+=2)
	    {
    		aes_decrypt(&ctx, (unsigned char *) &lljpg[i], (unsigned char *) &lljpg2[i]);
			lljpg2[i] ^= old_enc[0];
			lljpg2[i+1] ^= old_enc[1];
			old_enc[0] = lljpg[i];
			old_enc[1] = lljpg[i+1];
	    }
		jpg_size -= sizeof(uint64)*2+sizeof(int);
		return jpg_size;
	} else
		return -1;
}

void jpe_view(char *fname)
{
    uchar *jpg;
    int jpg_size, c, quit;

	generic_image();

    jpg = (uchar *) PTR;
	jpg_size = decrypt_image(fname, jpg, 120*1024);

	if (jpg_size > 0)
		joint_view(jpg, jpg_size);
	else {
		quit = 0;
	    while (!quit)
		{
			while ((c = getchar()) == EOF);
			switch(c&0x7f)
			{
			case RAWKEY_A:
			case RAWKEY_B:
				if(c != (RAWKEY_A|0x80))
					quit = 1;
				break;
			}
		}
	}
}

void jpg_view(char *fname)
{
	joint_view(file2mem(fname, NULL, 0),0);
}

void joint_view(uchar *jpg, int jpg_ram_usage)
{
	int c,fd, quit = 0, r, l;
	int x=0, y=0, scale=0;
	int dx=0, dy=0;
	int speed = 8;
	unsigned short *p = (unsigned short *)0x06000000;

	r = prepare_jpg(jpg, jpg_ram_usage);

	if (r == 2) {
		r = 40;
		while (r--) {
			l = 240*160;
			while(l--)
				*p++ = 0x7fff;
		}
	} else if (r) {
	
		l = 240*160;
		while(l--)
			*p++ = 0;
	
		render_jpg(x, y, scale);
		while(!quit)
		{
			if ((dx || dy) && !scale)
				render_jpg(x, y, scale);

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
				if (c != (RAWKEY_SELECT|0x80)) {
					p = (unsigned short *)0x06000000;
					l = 240*160;
					while(l--)
						*p++ = 0x0;
					scale=(scale+1)%3;
					render_jpg(x, y, scale);
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
			if((x+240) > jpg_w)
				x = (jpg_w-240);
			if((y+160) > jpg_h)
				y = (jpg_h-160);

			if(x < 0) x = 0;
			if(y < 0) y = 0;
		}
	}

	fd = open("/dev/screen", 0);
	ioctl(fd, SC_SETMODE, 2);
	close(fd);
}
