
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

void bitmap_free(BitMap *bm)
{
	if(bm->format & VIDEOMEM)
	{
		vmem = bm->pixels;
		return;
	}

	free(bm->pixels);
}

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

void bitmap_addbox(BitMap *dst, Rect *r, uint16 col)
{
	int w;
	uint16 *d = (uint16 *)dst->pixels + r->x + r->y * dst->width;
	int dmod = (dst->width - r->w);
	int dh = r->h;
	int high, low;

	while(dh--)
	{
		w = r->w;
		while(w--)
		{
			high = *d & 0x739c;
			low = *d & 0x0c63;
			high += (col & 0x739c);
			low += (col & 0x0c63);
			*d = (high+(low&0x318c))>>1;
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

void bitmap_fillrange(BitMap *bm, Rect *r, Color *color0, Color *color1, uint16 style)
{
	uint16 *d;
	int x,y,w,h,bmw;
	uint16 c0, c1, c2, d0, d1, d2, col16;

	c0 = (color0->b << 7);
	c1 = (color0->g << 2);
	c2 = (color0->r << 2);

	if(style & STYLE_HRANGE)
	{
		d0 = ( (color1->b - color0->b) << 7) / (r->w);
		d1 = ( (color1->g - color0->g) << 2) / (r->w);
		d2 = ( (color1->r - color0->r) << 2) / (r->w);

		bmw = bm->width;

		d = (uint16 *)bm->pixels + r->x + r->y * bmw;

		for(x=0; x<r->w; x++)
		{
			col16 = (c0 & 0x7C00) | (c1 & 0x03E0) | ((c2 >> 5) & 0x1F);

			c0 += d0;
			c1 += d1;
			c2 += d2;

			h = r->h;

			while(h--)
			{
				*d = col16;
				d += bmw;
			}


			d -= (bmw * r->h - 1);
		}
	}
	else
	{
		d0 = ( (color1->b - color0->b) << 7) / (r->h);
		d1 = ( (color1->g - color0->g) << 2) / (r->h);
		d2 = ( (color1->r - color0->r) << 2) / (r->h);


		d = (uint16 *)bm->pixels + r->x + r->y * bm->width;

		for(y=0; y<r->h; y++)
		{
			col16 = (c0 & 0x7C00) | (c1 & 0x03E0) | ((c2 >> 5) & 0x1F);

			c0 += d0;
			c1 += d1;
			c2 += d2;

			w = r->w;

			while(w--)
				*d++ = col16;

			d += (bm->width - r->w);
		}

	}

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
