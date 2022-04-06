
#include "pogo.h"
#include "bitmap.h"
#include "misc.h"

#define VRAM_BASE ((uint16 *)0x06000000)

static BitMap screenBitmap = {240, 160, BPP16, VRAM_BASE};
static uint16 *vmem = VRAM_BASE + 240*160;

BitMap *bitmap_new(int w, int h, int fmt)
{
	BitMap *b;

	if(((w | h) == 0) && (fmt == VIDEOMEM))
		return &screenBitmap;

	b = malloc(sizeof(BitMap));
	b->width = w;
	b->height = h;
	b->format = fmt;

	if(fmt & VIDEOMEM)
	{
		b->pixels = vmem;
		vmem += (w * h * fmt);
	}
	else
	if(!(fmt & DONT_ALLOC))
	{
		b->pixels = malloc(w * h * (fmt & 3));
	}
	else
	{
		b->format &= ~DONT_ALLOC;
		b->pixels = NULL;
	}

	return b;
}

#if 0
void bitmap_free(BitMap *bm)
{
	if(bm->format & VIDEOMEM)
	{
		vmem = bm->pixels;
		return;
	}

	free(bm->pixels);
}
#endif

void bitmap_clear(BitMap *dst, int val)
{
	int l = dst->width * dst->height;
	uint16 *d = (uint16 *)dst->pixels;
	while(l--)
		*d++ = val;
}

void bitmap_blit(BitMap *dst, int dx, int dy, BitMap *src, int sx, int sy, int sw, int sh)
{
	int w, tsw;
	uint16 *savew, *saveh;
	uint16 *s = (uint16 *)src->pixels + sx + sy * src->width;
	uint16 *d = (uint16 *)dst->pixels + dx + dy * dst->width;
	int smod = (src->width - sw);
	int dmod = (dst->width - sw);

	if((sw > src->width) || (sh > src->height))
	{
		// Tiling blit

		int h;

		saveh = s;

		while(sh > 0)
		{
			h = src->height;
			if(h > sh)
				h = sh;

			while(h--)
			{
				tsw = sw;
				savew = s;
				while(tsw > 0)
				{
					s = savew;

					w = src->width;
					if(tsw < w)
						w = tsw;

					while(w--)
						*d++ = *s++;


					tsw -= src->width;
				}

				d += dmod;

				s = savew;
				s += src->width;
			}

			s = saveh;


			sh -= src->height;

			//s = (uint16 *)src->pixels + sx + sy * src->width;

		}
	}
	else
	{
		if(src->format & TRANSPARENT)
		{
			while(sh--)
			{
				w = sw;
				while(w--)
				{
					if(!(*s & 0x8000)) *d = *s;
					d++;
					s++;
				}
				d += dmod;
				s += smod;
			}
		}
		else
		{
			while(sh--)
			{
				w = sw;
				while(w--)
					*d++ = *s++;
				d += dmod;
				s += smod;
			}
		}
	}
}

void bitmap_fillbox(BitMap *dst, Rect *r, uint16 col)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;

	while(dh--)
	{
		w = r->w;
		while(w--)
			*d++ = col;
		d += dmod;
	}
}

void bitmap_avgbox(BitMap *dst, Rect *r, uint16 col)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	uint16 colh, coll;
	uint32 high, low;

	colh = col & 0x739c;
	coll = col & 0x0c63;
	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = *d & 0x739c;
			low = *d & 0x0c63;
			high += colh;
			low += coll;
			*d = (high+(low&0x18c6))>>1;
			d++;
		}

		d += dmod;
	}
}

void bitmap_backbox(BitMap *dst, Rect *r, uint16 col, uint16 shift)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	uint16 mask16;
	uint32 high, low, mask;

	mask16 = (0x1f >> shift);
	mask16 |= (mask16 << 5) | (mask16 << 10);
	col = (col>>shift) & mask16;
	low = (col&0x1f) | ((col&(0x1f<<5))<<1) | ((col&(0x1f<<10))<<2);
	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = *d;
			high = (high&0x1f) | ((high&(0x1f<<5))<<1) | ((high&(0x1f<<10))<<2);
			high += low;
			mask = high & ((1<<5) | (1<<11) | (1<<17));
			mask >>= 1;
			mask |= (mask>>1);
			mask |= (mask>>2);
			mask |= (mask>>1);
			high |= mask;
			*d = (high&0x1f) | ((high&(0x1f<<6))>>1) | ((high&(0x1f<<12))>>2);
			d++;
		}

		d += dmod;
	}
}

void bitmap_colorbox(BitMap *dst, Rect *r, uint16 col, uint16 shift)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	uint16 mask16;
	uint32 high, low, mask;

	mask16 = (0x1f >> shift);
	mask16 |= (mask16 << 5) | (mask16 << 10);
	low = (col&0x1f) | ((col&(0x1f<<5))<<1) | ((col&(0x1f<<10))<<2);
	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = (*d >> shift) & mask16;
			high = (high&0x1f) | ((high&(0x1f<<5))<<1) | ((high&(0x1f<<10))<<2);
			high += low;
			mask = high & ((1<<5) | (1<<11) | (1<<17));
			mask >>= 1;
			mask |= (mask>>1);
			mask |= (mask>>2);
			mask |= (mask>>1);
			high |= mask;
			*d = (high&0x1f) | ((high&(0x1f<<6))>>1) | ((high&(0x1f<<12))>>2);
			d++;
		}

		d += dmod;
	}
}

void bitmap_negbox(BitMap *dst, Rect *r, uint16 col)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	uint32 high, low, mask;

	low = (col&0x1f) | ((col&(0x1f<<5))<<1) | ((col&(0x1f<<10))<<2);
	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = *d;
			high = (high&0x1f) | ((high&(0x1f<<5))<<1) | ((high&(0x1f<<10))<<2);
			high |= (1<<5) | (1<<11) | (1<<17);
			high -= low;
			mask = high & ((1<<5) | (1<<11) | (1<<17));
			mask >>= 1;
			mask |= (mask>>1);
			mask |= (mask>>2);
			mask |= (mask>>1);
			high &= mask;
			*d = (high&0x1f) | ((high&(0x1f<<6))>>1) | ((high&(0x1f<<12))>>2);
			d++;
		}

		d += dmod;
	}
}

void bitmap_addbox(BitMap *dst, Rect *r, uint16 col)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	uint32 high, low, mask;

	low = (col&0x1f) | ((col&(0x1f<<5))<<1) | ((col&(0x1f<<10))<<2);
	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = *d;
			high = (high&0x1f) | ((high&(0x1f<<5))<<1) | ((high&(0x1f<<10))<<2);
			high += low;
			mask = high & ((1<<5) | (1<<11) | (1<<17));
			mask >>= 1;
			mask |= (mask>>1);
			mask |= (mask>>2);
			mask |= (mask>>1);
			high |= mask;
			*d = (high&0x1f) | ((high&(0x1f<<6))>>1) | ((high&(0x1f<<12))>>2);
			d++;
		}
		d += dmod;
	}
}

enum
{
	STYLE_HRANGE = 1,
	STYLE_VRANGE = 2,
};

void bitmap_fillsubrange(BitMap *bm, Rect *r, Rect *subr, Color *color0, Color *color1, uint16 style)
{
	uint16 *d;
	int bmw;
	int outer, inner, i, i0, i00, i1, j, j0, j00, j1, diff, diff2, diff3;
	uint16 c0, c1, c2, d0, d1, d2, col16;

	c0 = (color0->b << 7);
	c1 = (color0->g << 2);
	c2 = (color0->r << 2);

	bmw = bm->width;

	d = (uint16 *)bm->pixels + r->x + r->y * bmw;

	if(style & STYLE_HRANGE) {
		outer = r->w;
		inner = r->h;
		i0 = r->x;
		i00 = subr->x;
		i1 = subr->x + subr->w;
		j0 = r->y;
		j00 = subr->y;
		j1 = subr->y + subr->h;
		diff = bmw;
		diff2 = -(bmw * r->h - 1);
		diff3 = 1;
	} else {
		outer = r->h;
		inner = r->w;
		i0 = r->y;
		i00 = subr->y;
		i1 = subr->y + subr->h;
		j0 = r->x;
		j00 = subr->x;
		j1 = subr->x + subr->w;
		diff = 1;
		diff2 = (bmw - r->w);
		diff3 = bmw;
	}

	d0 = ( (color1->b - color0->b) << 7) / (outer);
	d1 = ( (color1->g - color0->g) << 2) / (outer);
	d2 = ( (color1->r - color0->r) << 2) / (outer);

	for(i=0; i<outer; i++)
	{
		col16 = (c0 & 0x7C00) | (c1 & 0x03E0) | ((c2 >> 5) & 0x1F);

		c0 += d0;
		c1 += d1;
		c2 += d2;

		if (i0 + i >= i00 && i0 + i < i1) {
			for (j = 0; j < inner; j++)
			{
				if (j0 + j >= j00 && j0 + j < j1)
				{
					*d = col16;
				}
				d += diff;
			}
			d += diff2;
		} else
			d += diff3;
	}
}

void bitmap_fillrange(BitMap *bm, Rect *r, Color *color0, Color *color1, uint16 style)
{
	bitmap_fillsubrange(bm, r, r, color0, color1, style);
}


BitMap *bitmap_readbmp(FILE *fp)
{
	BitMap *bm = NULL;
	uint16 colors[256];
	uint16 *ptr;
	int start, size, width, height, i, j, c;

	fseek(fp, 10, SEEK_SET);
	fread(&start, 4, 1, fp);
	fread(&size, 4, 1, fp);
	fread(&width, 4, 1, fp);
	fread(&height, 4, 1, fp);
	fseek(fp, size+14, SEEK_SET);

	//fprintf(stderr, "bmp %d x %d\n", width, height);

	for(i=0; i<256; i++)
	{
		uchar col[4];
		fread(col, 4, 1, fp);
		colors[i] = COL16(col);
	}

	fseek(fp, start, SEEK_SET);

	bm = bitmap_new(width, height, BPP16);

	ptr = (uint16 *)bm->pixels + width * (height-1);

	while(height--)
	{
		for(j=0; j<width; j++)
		{
			c = fgetc(fp);
			*ptr++ = colors[c];
		}

		ptr -= (width * 2);
	}

	return bm;
}

BitMap *bitmap_readbm(FILE *fp)
{
	BitMap *bm;
	uint16 *mem;
	uint16 width, height;

	if((mem = (uint16 *)fseek(fp, 0, SEEK_MEM)))
	{
		width = mem[2];
		height = mem[3];
		bm = malloc(sizeof(BitMap));
		bm->format = BPP16;
		bm->width = width;
		bm->height = height;
		bm->pixels = &mem[4];
	}
	else
	{
		fseek(fp, 4, SEEK_SET);
		fread(&width, 2, 1, fp);
		fread(&height, 2, 1, fp);

		bm = bitmap_new(width, height, BPP16);
		fread(bm->pixels, width*height, 2, fp);
	}

	return bm;

}

BitMap *bitmap_loadbm(char *name)
{
	BitMap *bm;
	FILE *fp;
	char tmp[80];
	strcpy(tmp, GET_PATH(BITMAPS));
	strcat(tmp, name);
	//sprintf(tmp, ".shell/bitmaps/%s", name);
	fp = fopen(tmp, "rb");
	bm = bitmap_readbm(fp);
	fclose(fp);
	return bm;
}
