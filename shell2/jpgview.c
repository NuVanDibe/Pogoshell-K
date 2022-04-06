
#include <pogo.h>
#include "jpgview.h"
#include "bitmap.h"
#include "window.h"
#include "misc.h"
#include "iwram.h"
#include "aes.h"

extern Screen *MainScreen;
JPEG_OUTPUT_TYPE *jpg_ptr;
int jpg_w;
int jpg_h;

Font *text;

void jpgviewer_set_font(Font *f)
{
	text = f;
}

void joint_view(uchar *jpg);

#define BG_PALRAM ((uint16*)0x05000000)

int prepare_jpg(unsigned char *ptr)
{
	int sfd;

	sfd = open("/dev/screen", 0);
	ioctl(sfd, SC_SETMODE, 2);
	close(sfd);

    return JPEG_DecompressImage(ptr, &jpg_ptr, &jpg_w, &jpg_h);
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
			color = ((x*y)>>9) + (((119-x)*(79-y))>>9) + ((x+y)>>4);
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
	if (c & 0x8000)
		c--;

	for (h = 0; h < 8; h++)
	    for (w = 0; w < 8; w++)
			dest[(y+h)*240+x+w] = c;
}

int decrypt_image(char *fname, uchar **jpg)
{
	int c, done, jpg_size, x, y, shift_count;
    uint64 key[2], old_enc[2], *lljpg, *out, i;
	uchar *enc;
	static uint64 old_key[2] = {0, 0};
	static int shift = 0;
	aes_context ctx;
    
	enc = file2mem(fname, NULL, 0, RAW);
	jpg_size = *(int *)enc;

	pfree();
	out = pmalloc((jpg_size+16)&~15);
	*jpg = out;

    if (out && jpg_size > sizeof(uint64)*2) {
	    lljpg = (uint64 *) &enc[sizeof(int)];

    	key[0] = key[1] = 0;
		shift_count = 0;

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
					shift_count += 3;
					break;
				case RAWKEY_DOWN:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 1;
					shift_count += 3;
					break;
				case RAWKEY_LEFT:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 2;
					shift_count += 3;
					break;
				case RAWKEY_RIGHT:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 3;
					shift_count += 3;
					break;
				case RAWKEY_L:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 4;
					shift_count += 3;
					break;
				case RAWKEY_R:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 5;
					shift_count += 3;
					break;
				case RAWKEY_A:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 6;
					shift_count += 3;
					break;
				case RAWKEY_B:
					draw_block(x, y);
					x += 8;
					key[1] = (key[1]<<3) | (key[0]>>61);
					key[0] <<= 3;
					key[0] |= 7;
					shift_count += 3;
					break;
				case RAWKEY_SELECT:
					shift_count = shift;
					key[0] = old_key[0];
					key[1] = old_key[1];
					for (i = 0; i < /*128/3*/43; i++)
					{
						draw_block(x, y);
						x += 8;
						if (x == 240) {
							x = 0;
							y += 8;
						}
						if (y == 160) {
							y = 0;
						}
					}
					break;
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
		shift = (shift_count > 128) ? 128 : shift_count;
		old_key[0] = key[0];
		old_key[1] = key[1];
		if (shift < 128) {
			if (shift < 64) {
				key[1] |= (lljpg[1]<<shift);
				key[1] |= (lljpg[0]>>(64-shift));
				key[0] |= (lljpg[0]<<shift);
			} else {
				key[1] |= (lljpg[0]<<(shift-64));
			}
		}
		aes_set_key(&ctx, (unsigned char *) key, 16, 0);
		old_enc[0] = old_enc[1] = 0xfedcba9876543210ll;
		lljpg += 2;
    	for (i = 0; i < (jpg_size/sizeof(uint64)); i+=2)
	    {
    		aes_decrypt(&ctx, (unsigned char *) &lljpg[i], (unsigned char *) &out[i]);
			out[i] ^= old_enc[0];
			out[i+1] ^= old_enc[1];
			old_enc[0] = lljpg[i];
			old_enc[1] = lljpg[i+1];
	    }
		return jpg_size;
	} else
		return -1;
}

void jpe_view(char *fname)
{
    uchar *jpg;
    int jpg_size, c, quit;

	generic_image();

	jpg_size = decrypt_image(fname, &jpg);

	if (jpg && jpg_size > 0)
		joint_view(jpg);
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
	uchar *ptr;

	ptr = file2mem(fname, NULL, 0, RAW);

	pfree();
	joint_view(ptr);
}

#define MIN(a,b) (a<b ? a : b)
#define MAX(a,b) (a>b ? a : b)

#define OFFSET 8
int num[]={1,  3,1, 3,1,3,1,3,1,3,2,3,4,6,8,12,16};
int den[]={16,32,8,16,4,8,2,4,1,2,1,1,1,1,1,1,1};

void joint_view(uchar *jpg)
{
	int c,fd, quit = 0, r, l;
	int x=0, y=0, scale=0, corscale, mode=0, rotate=0, toscale=0, toshift=0;
	int dx=0, dy=0;
	int speed = 8, change1=1, change2=0, oldx=-1, oldy=-1;
	unsigned short *p;
	int w, h;
	int wi, hi;
	int edgew, edgeh;
	int count = 0;
	int dirty = 1, scalechange = 1;
	char buffer[13];
	u16 *dst;
	BitMap *bm;

	r = prepare_jpg(jpg);

	if (r == 2) {
		r = 40;
		while (r--) {
			p = (unsigned short *)0x06000000;
			l = 240*160;
			while(l--)
				*p++ = 0x7fff;
		}
	} else if (r) {
		while(!quit)
		{
			if (((oldx != x || oldy != y || change1) && !mode) ||
				(change2)) {
				switch(mode)
				{
					case 1: //Aspect
						if (rotate&1) {
							wi = 160;
							hi = 160 * jpg_h /jpg_w;
							if (hi > 240) {
								hi = 240;
								wi = 240 * jpg_w / jpg_h;
							}
						} else {
							wi = 240;
							hi = 240 * jpg_h /jpg_w;
							if (hi > 160) {
								hi = 160;
								wi = 160 * jpg_w / jpg_h;
							}
						}
						sprintf(buffer, "%d%%", (int) (hi*100/jpg_h));
						w = wi;
						h = hi;
						break;
					case 2: //Stretch
						if (rotate&1) {
							w = wi = 160;
							h = hi = 240;
						} else {
							w = wi = 240;
							h = hi = 160;
						}
						sprintf(buffer, "%d%%x%d%%", (int) (wi*100/jpg_w), (int) (hi*100/jpg_h));
						break;
					default:
						corscale = scale + OFFSET;
						wi = jpg_w*num[corscale]/den[corscale];
						hi = jpg_h*num[corscale]/den[corscale];
						if (rotate&1) {
							edgew = 160;
							edgeh = 240;
							w = MIN(160, wi);
							h = MIN(240, hi);
						} else {
							edgew = 240;
							edgeh = 160;
							w = MIN(240, wi);
							h = MIN(160, hi);
						}
						edgew = edgew*den[corscale]/num[corscale];
						edgeh = edgeh*den[corscale]/num[corscale];
						corscale = scale + OFFSET;
						sprintf(buffer, "%d%%", (int) (num[corscale]*100/den[corscale]));

						if((x+edgew) > jpg_w)
							x = (jpg_w-edgew);
						if((y+edgeh) > jpg_h)
							y = (jpg_h-edgeh);
						if(x < 0) x = 0;
						if(y < 0) y = 0;

						break;
				}
				render_jpg(x, y, w, h, wi, hi, mode, scale, rotate, toshift);
				dirty = 1;
				count = 0;
			}
			change1 = change2 = 0;
			oldx = x;
			oldy = y;

			c = getchar();

			count++;
			Halt();
			if (count > 60*4) {
				if (scalechange) {
					scalechange = 0;
					render_jpg(x, y, w, h, wi, hi, mode, scale, rotate, toshift);
				}
				count = 0;
			}

			if (scalechange && dirty) {
				dirty = 0;
				bm = MainScreen->bitmap;
				l = font_text(text, buffer, NULL, bm->width);
 				dst = (uint16 *)bm->pixels + (bm->width - l);
				font_setcolor(0x7fff, 0);
				font_text(text, buffer, dst, bm->width);
			}

			switch(c&0x7F)
			{
			case RAWKEY_L:
				speed = c&0x80 ? 8 : 128;
				break;
			case RAWKEY_R:
				toscale = c&0x80 ? 0 : 1;
				break;
			case RAWKEY_RIGHT:
				if (toscale) {
					if (c != (RAWKEY_RIGHT|0x80)) {
						rotate = (rotate+1)&3;
						change2 = 1;
						scalechange = dirty = 1;
						count = 0;
					}
				} else {
					if (rotate&1) {
						dy = (((rotate&2)^2)-1)*(c&0x80 ? 0 : speed);
					} else {
						dx = ((rotate^2)-1)*(c&0x80 ? 0 : speed);
					}
				}
				break;
			case RAWKEY_LEFT:
				if (toscale) {
					if (c != (RAWKEY_LEFT|0x80)) {
						rotate = (rotate+3)&3;
						change2 = 1;
						scalechange = dirty = 1;
						count = 0;
					}
				} else {
					if (rotate&1) {
						dy = ((rotate&2)-1)*(c&0x80 ? 0 : speed);
					} else {
						dx = (rotate-1)*(c&0x80 ? 0 : speed);
					}
				}
				break;
			case RAWKEY_UP:
				if (toscale) {
					if (c != (RAWKEY_UP|0x80)) {
						scale++;
						scale = (scale > OFFSET) ? OFFSET : scale;
						change1 = 1;
						scalechange = dirty = 1;
						count = 0;
					}
				} else {
					if (rotate&1) {
						dx = (((rotate&2)^2)-1)*(c&0x80 ? 0 : speed);
					} else {
						dy = (rotate-1)*(c&0x80 ? 0 : speed);
					}
				}
				break;
			case RAWKEY_DOWN:
				if (toscale) {
					if (c != (RAWKEY_DOWN|0x80)) {
						scale--;
						scale = (scale < -OFFSET) ? -OFFSET : scale;
						change1 = 1;
						scalechange = dirty = 1;
						count = 0;
					}
				} else {
					if (rotate&1) {
						dx = ((rotate&2)-1)*(c&0x80 ? 0 : speed);
					} else {
						dy = ((rotate^2)-1)*(c&0x80 ? 0 : speed);
					}
				}
				break;
			case RAWKEY_SELECT:
				if (c != (RAWKEY_SELECT|0x80)) {
					mode=(mode+1)%3;
					change2 = 1;
					scalechange = dirty = 1;
					count = 0;
				}
				break;
			case RAWKEY_START:
				if (toscale) {
					if (c != (RAWKEY_START|0x80)) {
						toshift ^= 1;
						change2 = 1;
					}
				} else {
					if (c != (RAWKEY_START|0x80)) {
						scale = 0;
						change1 = 1;
						scalechange = dirty = 1;
						count = 0;
					}
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
		}
	}

	fd = open("/dev/screen", 0);
	ioctl(fd, SC_SETMODE, 2);
	close(fd);
}
