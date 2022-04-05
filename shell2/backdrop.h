#ifndef BACKDROP_H
#define BACKDROP_H

#include "bitmap.h"

enum
{
	STYLE_SOLID, // Single color (color[2])
	STYLE_HRANGE = 1, // Horizontal range between color[2] and color[3]
	STYLE_VRANGE = 2, // Vertical range between color[2] and color[3]
	STYLE_HVRANGE = 3,
	STYLE_BEVEL = 4, // Border color is color[0] up & left and color[1] down & right
	STYLE_BITMAP = 8, // color[2] & color[3] unused, stretched bitmap in center
	STYLE_BMHSTRETCH = 9, // Stretch instead of tile in horizontal direction
	STYLE_BMVSTRETCH = 10, // Stretch instead of tile in vertical direction
	STYLE_CUSTOM = 32,
};

enum { COLOR_BORDER0, COLOR_BORDER1, COLOR_SOLID0, COLOR_SOLID1 };


typedef struct
{
	uint16 style;
	uchar border;
	uchar something;
	Color color[4];
	BitMap *bitmap;

} BackDrop;

typedef void (*CustomDrawFunc)(BackDrop *, Rect *, BitMap *);

void backdrop_render(BackDrop *bd, Rect *r, BitMap *bm);
BackDrop *backdrop_new(uint16 style);
void backdrop_set_bitmap(BackDrop *bd, BitMap *bm);
void backdrop_set_color(BackDrop *bd, Color *color, int n);
void backdrop_set_border(BackDrop *bd, int border);

void backdrop_set_attribute(BackDrop *bd, int attr, void *val);

#endif
