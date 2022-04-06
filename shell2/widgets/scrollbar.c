
#include "../settings.h"
#include "../bitmap.h"
#include "../window.h"
#include "scrollbar.h"

int scrollbar_render(Scrollbar *scr, Rect *r, BitMap *bm)
{
	Rect sbar;

	if (settings_get(SF_SCROLLBAR)) {
		sbar.w = scr->w.width;
		sbar.x = r->x;
		r->w -= sbar.w;
		if (scr->alignside) {
			sbar.x = r->x + r->w;
		} else {
			sbar.x = 0;
			r->x += sbar.w;
		}
	}

	if (scr->w.flags & WFLG_REDRAW) {
		scr->w.flags &= ~WFLG_REDRAW;

		if (settings_get(SF_SCROLLBAR)) {
			int i;

			sbar.x += scr->marginx[0];
			sbar.w -= scr->marginx[0] * 2;
			sbar.y = r->y + scr->marginy[0];
			sbar.h = r->h - scr->marginy[0] * 2;

			i = scr->lines;
			if(i < scr->showing)
				i = scr->showing;

			if ((scr->style & TROUGH_ALWAYS) ||
				((scr->style & TROUGH_PARTIAL) && (scr->lines > scr->showing)))
			{
				Color c;

				c = scr->troughcolor;
				if (c.a == 0xFF)
					bitmap_addbox(bm, &sbar, TO_RGB16(c));
				else if (c.a == 0xFE)
					bitmap_negbox(bm, &sbar, TO_RGB16(c));
				else if (c.a > 0x7F)
					bitmap_avgbox(bm, &sbar, TO_RGB16(c));
				else
					bitmap_fillbox(bm, &sbar, TO_RGB16(c));
			}

			if (scr->bar &&
				((scr->style & BAR_ALWAYS) ||
				 ((scr->style & BAR_PARTIAL) && (scr->lines > scr->showing))))
			{
				sbar.x += scr->marginx[1];
				sbar.w -= scr->marginx[1] * 2;
				sbar.y += scr->marginy[1];
				sbar.h -= scr->marginy[1] * 2;

				sbar.y = sbar.y + scr->start * sbar.h / i;
				sbar.h = sbar.h * scr->showing / i;

				backdrop_render(scr->bar, &sbar, bm);
			}
		}
		return 1;
	}
	return 0;
}

Scrollbar *scrollbar_new()
{
	uint32 *p;

	Scrollbar *sc = malloc(sizeof(Scrollbar));

	render_functions[WIDGET_SCROLLBAR & 0xFFF] = (WidgetRenderFunc)scrollbar_render;

	sc->w.type = WIDGET_SCROLLBAR;
	sc->w.height = 0;
	sc->w.width = 10;

	sc->w.flags = WFLG_REDRAW;

	sc->alignside = ALIGN_RIGHT;
	sc->marginx[0] = 1;
	sc->marginx[1] = 1;
	sc->marginy[0] = 1;
	sc->marginy[1] = 1;
	sc->style = TROUGH_ALWAYS | BAR_ALWAYS;

	p = (uint32 *)&sc->troughcolor;

	p[0] = 0xff181818;
	sc->bar = NULL;

	return sc;

}

void scrollbar_set_dirty(Scrollbar *sc)
{
	sc->w.flags |= WFLG_REDRAW;
}

void scrollbar_set_start(Scrollbar *sc, int start)
{
	if (start < 0 || start >= sc->lines)
		return;

	if (sc->start != start) {
		sc->start = start;
		sc->w.flags |= WFLG_REDRAW;
	}
}

void scrollbar_set_showing(Scrollbar *sc, unsigned int showing)
{
	if (sc->showing != showing) {
		sc->showing = showing;
		sc->w.flags |= WFLG_REDRAW;
	}
}

void scrollbar_set_lines(Scrollbar *sc, unsigned int lines)
{
	if (sc->lines != lines) {
		sc->lines = lines;
		sc->w.flags |= WFLG_REDRAW;
	}
}

void scrollbar_set_attribute(Scrollbar *sc, int attr, void *val)
{
	Color c;
	int n = attr & 0xf;
	uint32 l;

	switch(attr & 0xFF0)
	{
	case WATR_BACKDROP:
		if (sc->bar)
			free(sc->bar);

		sc->bar = (BackDrop *)val;
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_COLWIDTH:
		sc->w.width = (int) val;
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_RGB:
		l = (int) val;
		c.r = (l>>16) & 0xff;
		c.g = (l>>8) & 0xff;
		c.b = l & 0xff;
		c.a = l>>24;

		sc->troughcolor = c;
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_COLOR:
		sc->troughcolor = *((Color *)val);
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_MARGIN:
		if (n & 1)
			sc->marginy[(n & 2)>>1] = (int) val;
		else
			sc->marginx[(n & 2)>>1] = (int) val;
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_ALIGN:
		sc->alignside = (int) val;
		sc->w.flags |= WFLG_REDRAW;
		break;
	case WATR_STYLE:
		sc->style &= ~(3<<(n<<1));
		sc->style |= (((int) val)<<(n<<1));
		sc->w.flags |= WFLG_REDRAW;
		break;
	}
}

