
#include <pogo.h>

#include "../bitmap.h"
#include "../window.h"
#include "textflow.h"

int textflow_render(TextFlow *tb, Rect *r, BitMap *bm)
{
	int i;
	uint16 *dst = (uint16 *)bm->pixels + (r->x+2) + (r->y+4) * bm->width;
	if(tb->w.flags & WFLG_REDRAW)
	{

		if(tb->backdrop)
		{
			backdrop_render(tb->backdrop, r, bm);
		}
		else
			bitmap_fillbox(bm, r, 0x6318);

		font_setcolor(TO_RGB16(tb->textcolor), 0x0000);

		for(i=0; i<tb->numlines; i++)
		{
			font_text(tb->font, tb->textline[i], dst + (r->w - 2 - tb->textlength[i])/2, bm->width);
			dst += (tb->font->height * bm->width);
		}
		font_setcolor(0, 0);

		tb->w.flags &= ~WFLG_REDRAW;
		return 1;
	}
	return 0;
}

int calc_lengths(TextFlow *tf)
{
	int i, maxl = 0;
	for(i=0; i<tf->numlines; i++)
	{
		tf->textlength[i] = font_text(tf->font, tf->textline[i], NULL, 240);
		if(tf->textlength[i] > maxl)
			maxl = tf->textlength[i];
//		fprintf(stderr, "%d \"%s\"\n", tf->textlength[i], tf->textline[i]);
	}

	return maxl;
}

void textflow_set_attribute(TextFlow *tb, int attr, void *val)
{
	int l;
	char *p;
	switch(attr & 0xFF0)
	{
	case WATR_BACKDROP:
		tb->backdrop = (BackDrop *)val;
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_COLOR:
		tb->textcolor = *((Color *)val);
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_RGB:
		l = (int)val;
		tb->textcolor.r = l>>16;
		tb->textcolor.g = (l>>8) & 0xff;
		tb->textcolor.b = l & 0xff;
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_TEXT:
		if(strcmp(tb->text, (char *)val) != 0)
		{
			char *linep, *endp;
			strcpy(tb->text, (char *)val);
			p = tb->text;
			endp = p + strlen(p);

			tb->numlines = 0;

			while(p < endp)
			{
				//fprintf(stderr, "%d: %s\n", tb->numlines, p);
				linep = tb->textline[tb->numlines++] = p;
				while(*p && *p != '\n') p++;
				*p++ = 0;

			}

			//fprintf(stderr, "%d LINES\n", tb->numlines);

			if(tb->font)
				tb->w.height = tb->font->height * tb->numlines;
			tb->w.width = calc_lengths(tb);

			tb->w.flags |= WFLG_REDRAW;
		}
		break;
	case WATR_FONT:
		if(tb->font != (Font *)val)
		{
			tb->font = (Font *)val;
			tb->w.flags |= WFLG_REDRAW;
			tb->w.height = tb->font->height * tb->numlines;
			tb->w.width = calc_lengths(tb);
		}
		break;
	default:
		//dprint("Illegal Attribute!!!\n");
		break;
	}
}


TextFlow *textflow_new(Font *font, int texlen)
{

	TextFlow *tb = malloc(sizeof(TextFlow));

	render_functions[WIDGET_TEXTFLOW & 0xFFF] = (WidgetRenderFunc)textflow_render;

	tb->w.type = WIDGET_TEXTFLOW;
	tb->w.height = font->height+2;
	tb->w.width = 0;
	tb->textcolor = Black_Color;
	tb->w.flags = WFLG_REDRAW;
	tb->backdrop = NULL;
	tb->text = malloc(texlen);
	strcpy(tb->text, "");
	tb->font = font;
	tb->numlines = 0;

	return tb;
}
