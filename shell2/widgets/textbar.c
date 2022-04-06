
#include <pogo.h>

#include "../bitmap.h"
#include "../window.h"
#include "textbar.h"

int textbar_render(TextBar *tb, Rect *org_r, BitMap *bm)
{
	Rect cr = *org_r;
	Rect *r = &cr;
	Font *f = tb->typeface->font;

	int l;
	char left[256];
	char *right;
	uint16 *dst;
	strcpy(left, tb->text);
	right = left;
	while(*right && *right != '|') right++;
	if(*right)
		*right++ = 0;
	else
		right = NULL;

	if(tb->w.flags & WFLG_REDRAW)
	{
		if(tb->backdrop)
		{
			backdrop_render(tb->backdrop, r, bm);
		}
		else
			bitmap_fillbox(bm, r, 0x6318);

 		dst = (uint16 *)bm->pixels + (r->x+tb->margin) + (r->y+tb->margin) * bm->width;

		if(tb->align == ALIGN_CENTER)
		{
			l = font_text(f, left, NULL, bm->width);
			dst += ((r->w - l) / 2);
		}
		else
		if(tb->align == ALIGN_RIGHT)
		{
			l = font_text(f, left, NULL, bm->width);
			dst += (r->w - l - tb->margin);
		}

		font_setcolor(TO_RGB16(tb->textcolor), 0x0000);
		font_setshadowoutline(0x0000, 0x7fff);
		//dst += (bm->width + 1);
		font_text(f, left, dst, bm->width);
		if((tb->align == ALIGN_LEFT) && right)
		{
			l = font_text(f, right, NULL, bm->width);
			dst += (r->w - l - (tb->margin*2));
			font_text(f, right, dst, bm->width);
		}

		font_setcolor(0, 0);

		tb->w.flags &= ~WFLG_REDRAW;
		return 1;
	}
	return 0;
}

void textbar_set_attribute(TextBar *tb, int attr, void *val)
{
	Font *f;
	int l;

	switch(attr & 0xFFF0)
	{
	case WATR_BACKDROP:
		if (tb->backdrop)
			free(tb->backdrop);

		tb->backdrop = (BackDrop *)val;
		f = tb->typeface->font;
		if(f) {
			tb->w.height = font_height(f)+tb->margin*2;
			tb->w.width = font_text(f, tb->text, NULL, 240);
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
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
			f = tb->typeface->font;
			if(f) {
				tb->w.height = font_height(f)+tb->margin*2;
				tb->w.width = font_text(f, tb->text, NULL, 240);
				if (tb->backdrop) {
					tb->w.height += tb->backdrop->border*2;
					tb->w.width += tb->backdrop->border*2;
				}
			}
			tb->w.flags |= WFLG_REDRAW;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_FONT:
		if (!tb->typeface->global) {
			if (tb->typeface->font)
				free(tb->typeface->font);
			free(tb->typeface);
		}

		f = (Font *) val;
		tb->typeface = typeface_new(f, 0);
		tb->w.height = font_height(f)+tb->margin*2;
		tb->w.width = font_text(f, tb->text, NULL, 240);
		if (tb->backdrop) {
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_ALIGN:
		if(tb->align != (int)val)
		{
			tb->align = (int)val;
			tb->w.flags |= WFLG_REDRAW;
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	//case WATR_NAME:
	//	strcpy(tb->w.name, (char *)val);
	//	break;
	case WATR_MARGIN:
		tb->margin = (int)val;
		f = tb->typeface->font;
		if(f) {
			tb->w.height = font_height(f)+tb->margin*2;
			tb->w.width = font_text(f, tb->text, NULL, 240);
			if (tb->backdrop) {
				tb->w.height += tb->backdrop->border*2;
				tb->w.width += tb->backdrop->border*2;
			}
		}
		tb->w.flags |= WFLG_REDRAW;
		break;
	case WATR_TYPEFACE:
		if (!tb->typeface->global)
			free(tb->typeface);
		tb->typeface = (Typeface *)val;
		f = tb->typeface->font;
		tb->w.height = font_height(f)+tb->margin*2;
		tb->w.width = font_text(f, tb->text, NULL, 240);
		if (tb->backdrop) {
			tb->w.height += tb->backdrop->border*2;
			tb->w.width += tb->backdrop->border*2;
		}
		tb->w.flags |= WFLG_REDRAW;
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
	tb->typeface = typeface_new(font, 0);
	tb->w.height = (font) ? font_height(font) : 0;
	tb->w.height += tb->margin*2;
	tb->w.width = 0;
	tb->textcolor = Black_Color;
	tb->w.flags = WFLG_REDRAW;
	tb->backdrop = NULL;
	tb->text = malloc(texlen);
	strcpy(tb->text, "");
	tb->align = ALIGN_LEFT;

	return tb;
}
