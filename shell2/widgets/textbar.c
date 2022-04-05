
#include <pogo.h>

#include "../bitmap.h"
#include "../window.h"
#include "textbar.h"

int textbar_render(TextBar *tb, Rect *r, BitMap *bm)
{
	int l;
	char left[64];
	char *right;
	uint16 *dst = (uint16 *)bm->pixels + (r->x+tb->margin) + (r->y+tb->margin) * bm->width;
	strcpy(left, tb->text);
	right = left;
	while(*right && *right != '|') right++;
	if(*right)
		*right++ = 0;
	else
		right = NULL;

	if(tb->w.flags & WFLG_REDRAW)
	{
		if(tb->align == ALIGN_CENTER)
		{
			l = font_text(tb->font, left, NULL, bm->width);
			dst += ((r->w - l) / 2);
		}
		else
		if(tb->align == ALIGN_RIGHT)
		{
			l = font_text(tb->font, left, NULL, bm->width);
			dst += (r->w - l - tb->margin);
		}

		if(tb->backdrop)
		{
			backdrop_render(tb->backdrop, r, bm);
		}
		else
			bitmap_fillbox(bm, r, 0x6318);

		font_setcolor(TO_RGB16(tb->textcolor), 0x0000);
		//dst += (bm->width + 1);
		font_text(tb->font, left, dst, bm->width);
		if((tb->align == ALIGN_LEFT) && right)
		{
			l = font_text(tb->font, right, NULL, bm->width);
			dst += (r->w - l - (tb->margin*2));
			font_text(tb->font, right, dst, bm->width);
		}

		font_setcolor(0, 0);

		tb->w.flags &= ~WFLG_REDRAW;
		return 1;
	}
	return 0;
}

void textbar_set_attribute(TextBar *tb, int attr, void *val)
{
	int l;
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
			strcpy(tb->text, (char *)val);
			if(tb->font)
				tb->w.width = font_text(tb->font, tb->text, NULL, 240);
			tb->w.flags |= WFLG_REDRAW;
		}
		break;
	case WATR_FONT:
		if(tb->font != (Font *)val)
		{
			tb->font = (Font *)val;
			tb->w.flags |= WFLG_REDRAW;
			tb->w.height = tb->font->height+tb->margin*2;
			tb->w.width = font_text(tb->font, tb->text, NULL, 240);
		}
		break;
	case WATR_ALIGN:
		if(tb->align != (int)val)
		{
			tb->align = (int)val;
			tb->w.flags |= WFLG_REDRAW;
		}
		break;
	//case WATR_NAME:
	//	strcpy(tb->w.name, (char *)val);
	//	break;
	case WATR_MARGIN:
		tb->margin = (int)val;
		if(tb->font)
			tb->w.height = tb->font->height+tb->margin*2;
		break;
	default:
		//dprint("Illegal Attribute!!!\n");
		break;
	}
}

/*
void textbar_set_backdrop(TextBar *tb, BackDrop *bd)
{
	tb->backdrop = bd;
	tb->w.flags |= WFLG_REDRAW;
}

void textbar_set_color(TextBar *tb, Color *color)
{
	tb->textcolor = *color;
	tb->w.flags |= WFLG_REDRAW;
}
*/

TextBar *textbar_new(Font *font, int texlen)
{

	TextBar *tb = malloc(sizeof(TextBar));

	render_functions[WIDGET_TEXTBAR & 0xFFF] = (WidgetRenderFunc)textbar_render;

	tb->w.type = WIDGET_TEXTBAR;
	tb->margin = 1;
	tb->w.height = font->height+tb->margin*2;
	tb->w.width = 0;
	tb->textcolor = Black_Color;
	tb->w.flags = WFLG_REDRAW;
	tb->backdrop = NULL;
	tb->text = malloc(texlen);
	strcpy(tb->text, "");
	tb->align = ALIGN_LEFT;
	tb->font = font;

	return tb;
}
