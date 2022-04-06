
#include <pogo.h>

#include "../bitmap.h"
#include "../window.h"
#include "textflow.h"

int textflow_render(TextFlow *tb, Rect *org_r, BitMap *bm)
{
	Font *f;
	Rect cr = *org_r;
	Rect *r = &cr;

	int i, l;
	uint16 *dst;
	if(tb->w.flags & WFLG_REDRAW)
	{

		f = tb->typeface->font;
		if(tb->backdrop)
		{
			backdrop_render(tb->backdrop, r, bm);
		}
		else
			bitmap_fillbox(bm, r, 0x6318);

 		dst = (uint16 *)bm->pixels + (r->x+tb->marginl) + (r->y+tb->marginu) * bm->width;
		font_setcolor(TO_RGB16(tb->textcolor[0]), 0x0000);
		font_setshadowoutline(TO_RGB16(tb->typeface->shadow),TO_RGB16(tb->typeface->outline));

		for(i=0; i<tb->numlines; i++)
		{
			if (tb->align[0] == ALIGN_LEFT)
				l = 0;
			else if (tb->align[0] == ALIGN_RIGHT)
				l = r->w - tb->textlength[i] - tb->marginl - tb->marginr;
			else
				l = (r->w - tb->textlength[i] - tb->marginl - tb->marginr)/2;
			
			font_text(f, tb->textline[i], dst + l, bm->width);
			dst += (font_height(f) * bm->width);
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
	Font *f = tf->typeface->font;

	for(i=0; i<tf->numlines; i++)
	{
		tf->textlength[i] = font_text(f, tf->textline[i], NULL, 240);
		if(tf->textlength[i] > maxl)
			maxl = tf->textlength[i];
//		fprintf(stderr, "%d \"%s\"\n", tf->textlength[i], tf->textline[i]);
	}

	return maxl;
}

void textflow_set_attribute(TextFlow *tb, int attr, void *val)
{
	Font *f;
	int l;
	int n = attr&0xf;
	char *p;

	switch(attr & 0xFFF0)
	{
	case WATR_BACKDROP:
		if (tb->backdrop)
			free(tb->backdrop);

		tb->backdrop = (BackDrop *)val;

		f = tb->typeface->font;
		if(f) {
			tb->w.height = font_height(f) * tb->numlines + tb->marginu + tb->margind;
			tb->w.width = calc_lengths(tb) + tb->marginl + tb->marginr;
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_COLOR:
		tb->textcolor[n] = *((Color *)val);
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_RGB:
		l = (int)val;
		tb->textcolor[n].a = (l>>24) & 0xff;
		tb->textcolor[n].r = (l>>16) & 0xff;
		tb->textcolor[n].g = (l>>8) & 0xff;
		tb->textcolor[n].b = l & 0xff;
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_MARGIN:
		switch (n & 7)
		{
			case 1:
				tb->marginu = tb->margind = (int) val;
				break;
			case 2:
				tb->marginl = (int) val;
				break;
			case 3:
				tb->marginr = (int) val;
				break;
			case 4:
				tb->marginu = (int) val;
				break;
			case 5:
				tb->margind = (int) val;
				break;
			case 6:
				tb->marginlistx = (int) val;
				break;
			case 7:
				tb->marginlisty = (int) val;
				break;
			case 0:
			default:
				tb->marginl = tb->marginr = (int) val;
				break;
		}
		f = tb->typeface->font;
		if(f) {
			tb->w.height = font_height(f) * tb->numlines + tb->marginu + tb->margind;
			tb->w.width = calc_lengths(tb) + tb->marginl + tb->marginr;
			if (tb->backdrop) {
				tb->w.height += tb->backdrop->border*2;
				tb->w.width += tb->backdrop->border*2;
			}
		}
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

			f = tb->typeface->font;
			if(f) {
				tb->w.height = font_height(f) * tb->numlines + tb->marginu + tb->margind;
				tb->w.width = calc_lengths(tb) + tb->marginl + tb->marginr;
				if (tb->backdrop) {
					tb->w.height += tb->backdrop->border*2;
					tb->w.width += tb->backdrop->border*2;
				}
			}
			tb->w.flags |= WFLG_REDRAW;
		}
		break;
	case WATR_FONT:
		if (!tb->typeface->global) {
			if (tb->typeface->font)
				free(tb->typeface->font);
			free(tb->typeface);
		}

		f = (Font *) val;

		tb->typeface = typeface_new(f, 0);

		tb->w.height = font_height(f) * tb->numlines + tb->marginu + tb->margind;
		tb->w.width = calc_lengths(tb) + tb->marginl + tb->marginr;
		if (tb->backdrop) {
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_TYPEFACE:
		if (!tb->typeface->global)
			free(tb->typeface);

		tb->typeface = (Typeface *)val;
		f = tb->typeface->font;
		tb->w.height = font_height(f) * tb->numlines + tb->marginu + tb->margind;
		tb->w.width = calc_lengths(tb) + tb->marginl + tb->marginr;
		if (tb->backdrop) {
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_ALIGN:
		tb->align[n&1] = (int)val;
		tb->w.flags |= WFLG_REDRAW;
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
	tb->textcolor[0] = Black_Color;
	tb->textcolor[2] = White_Color;
	tb->textcolor[3] = Blue_Color;
	tb->w.flags = WFLG_REDRAW;
	tb->backdrop = NULL;
	tb->text = malloc(texlen);
	tb->marginlisty = tb->marginlistx = tb->marginl = tb->marginr = 2;
	tb->marginu = tb->margind = 4;
	tb->w.height = (font) ? font_height(font) : 0;
	tb->w.height += tb->marginu + tb->margind;
	tb->w.width = tb->marginl + tb->marginr;
	strcpy(tb->text, "");
	tb->typeface = typeface_new(font, 0);
	tb->align[0] = ALIGN_CENTER;
	tb->align[1] = ALIGN_LEFT;
	tb->numlines = 0;

	return tb;
}
