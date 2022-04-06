#ifndef BITMAP_H
#define BITMAP_H

#include <pogo.h>

typedef struct
{
	uint16 x;
	uint16 y;
	uint16 w;
	uint16 h;

} Rect;


typedef struct
{
	uchar r;
	uchar g;
	uchar b;
	uchar a;

} Color;


typedef struct
{
	uint16 width;
	uint16 height;
	uint32 format;
	void *pixels;

} BitMap;

enum
{
	BPP8 = 1,
	BPP16 = 2,
	DONT_ALLOC = 2048,
	TRANSPARENT = 4096,
	VIDEOMEM = 8192,
};

#define COL16(col) (((col[0]>>3)&0x1F)<<10)|(((col[1]>>3)&0x1F)<<5)|((col[2]>>3)&0x1F);

BitMap *bitmap_new(int w, int h, int fmt);

#define bitmap_getscreen() bitmap_new(0, 0, VIDEOMEM);

void bitmap_free(BitMap *bm);
void bitmap_blit(BitMap *dst, int dx, int dy, BitMap *src, int sx, int sy, int sw, int sh);
void bitmap_clear(BitMap *dst, int val);
BitMap *bitmap_readbmp(FILE *fp);
BitMap *bitmap_readbm(FILE *fp);
BitMap *bitmap_loadbm(char *name);

void bitmap_avgbox(BitMap *dst, Rect *r, uint16 col);
void bitmap_avgshiftbox(BitMap *dst, Rect *r, uint16 col, uint16 shift);
void bitmap_addshiftbox(BitMap *dst, Rect *r, uint16 col, uint16 shift);
void bitmap_addbox(BitMap *dst, Rect *r, uint16 col);
void bitmap_negbox(BitMap *dst, Rect *r, uint16 col);
void bitmap_fillbox(BitMap *dst, Rect *r, uint16 col);
void bitmap_fillsubrange(BitMap *bm, Rect *r, Rect *subr, Color *color0, Color *color1, uint16 style);
void bitmap_fillrange(BitMap *bm, Rect *r, Color *color0, Color *color1, uint16 style);

#endif
