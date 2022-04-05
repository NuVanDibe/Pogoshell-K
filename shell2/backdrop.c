
#include <pogo.h>

#include "bitmap.h"
#include "window.h"
#include "backdrop.h"

BackDrop *backdrop_new(uint16 style)
{
	BackDrop *bd = malloc(sizeof(BackDrop));
	bd->style = style;

	bd->color[0] = White_Color;
	bd->color[1] = Black_Color;
	bd->color[2] = Gray_Color;
	bd->color[3] = Gray_Color;

	bd->border = 0;
	bd->bitmap = NULL;

	return bd;
}

void backdrop_set_attribute(BackDrop *bd, int attr, void *val)
{
	int l;
	switch(attr & 0xFF0)
	{
	case WATR_BITMAP:
		bd->bitmap = (BitMap *)val;
		bd->style |= STYLE_BITMAP;
		break;
	case WATR_COLOR:
		bd->color[attr & 0xF] = *((Color *)val);
		break;
	case WATR_RGB:
		l = (int)val;
		bd->color[attr & 0xF].r = l>>16;
		bd->color[attr & 0xF].g = (l>>8) & 0xff;
		bd->color[attr & 0xF].b = l & 0xff;
		break;
	case WATR_BORDER:
		bd->border = (int)val;
		break;
	case WATR_STYLE:
		bd->style = (int)val;
		break;
	case WATR_NAME:
		break;
	default:
		//fprintf(stderr, "Illegal Attribute!!!\n");
		break;
	}
}

void backdrop_set_bitmap(BackDrop *bd, BitMap *bm)
{
	bd->style |= STYLE_BITMAP;
	bd->bitmap = bm;
}

void backdrop_set_color(BackDrop *bd, Color *color, int n)
{
	bd->color[n] = *color;
}

void backdrop_set_border(BackDrop *bd, int border)
{
	bd->border = border;
}

void backdrop_render(BackDrop *bd, Rect *r, BitMap *bm)
{
	Rect inner = *r;

	inner.x += bd->border;
	inner.y += bd->border;
	inner.w -= (bd->border * 2);
	inner.h -= (bd->border * 2);

	// Draw inner

	if(bd->style & STYLE_BITMAP)
	{
		bitmap_blit(bm, r->x, r->y, bd->bitmap, 0, 0, r->w, r->h);
	}
	else
	{
		if(bd->style & STYLE_HVRANGE)
		{
			bitmap_fillrange(bm, &inner, &bd->color[2], &bd->color[3], bd->style);
		}
		else
		{
			bitmap_fillbox(bm, &inner, TO_RGB16(bd->color[2]));
		}
	}

	if(bd->border)
	{
		int  y, w;
		int boffs, roffs;
		uint16 *d;
		uint16 col0 = TO_RGB16(bd->color[0]);
		uint16 col1 = TO_RGB16(bd->color[1]);

		boffs = (r->h - bd->border) * bm->width;
		roffs = (r->w - bd->border);

		d = (uint16 *)bm->pixels + r->x + r->y * bm->width;

		// Top/bottom part
		for(y=0; y<bd->border; y++)
		{
			w = r->w;
			while(w--)
			{
				d[boffs] = col1;
				*d++ = col0;
			}
			d += (bm->width - r->w);
		}

		// Middle part
		for(; y<(r->h - bd->border); y++)
		{
			w = bd->border;
			while(w--)
			{
				d[roffs] = col1;
				*d++ = col0;
			}
			d += (bm->width - bd->border);
		}
	}
}

