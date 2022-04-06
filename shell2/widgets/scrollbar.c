
#include "../settings.h"
#include "../bitmap.h"
#include "../window.h"
#include "scrollbar.h"

int scrollbar_render(Scrollbar *scr, Rect *r, BitMap *bm)
{
	int dirty, draw_trough, draw_bar, space_for_trough, space_for_bar;
	Rect sbar;

	dirty = draw_trough = draw_bar = space_for_trough = space_for_bar = 0;

	if (scr->w.flags & WFLG_REDRAW) {
		scr->w.flags &= ~WFLG_REDRAW;
		dirty = 1;
	}

	if (settings_get(SF_SCROLLBAR)) {
		int i;

		if ((scr->style & TROUGH_ALWAYS) == TROUGH_ALWAYS) {
			space_for_trough = 1;
			if (dirty)
				draw_trough = 1;
		} else if ((scr->style & TROUGH_PARTIAL) == TROUGH_PARTIAL) {
			space_for_trough = 1;
			if (dirty && (scr->lines > scr->showing))
				draw_trough = 1;
		} else if ((scr->style & TROUGH_MINIMAL) == TROUGH_MINIMAL) {
			if (scr->lines > scr->showing) {
				space_for_trough = 1;
				if (dirty)
					draw_trough = 1;
			}
		}

		if (scr->bar) {
			if ((scr->style & BAR_ALWAYS) == BAR_ALWAYS) {
				space_for_bar = 1;
				if (dirty)
					draw_bar = 1;
			} else if ((scr->style & BAR_PARTIAL) == BAR_PARTIAL) {
				space_for_bar = 1;
				if (dirty && (scr->lines > scr->showing))
					draw_bar = 1;
			} else if ((scr->style & BAR_MINIMAL) == BAR_MINIMAL) {
				if (scr->lines > scr->showing) {
					space_for_bar = 1;
					if (dirty)
						draw_bar = 1;
				}
			}
		}

		sbar.w = scr->w.width;
		if (scr->alignside)
			sbar.x = r->x + r->w - sbar.w;
		else
			sbar.x = 0;

		if (space_for_trough) {
			r->w -= sbar.w;
			if (!scr->alignside)
				r->x += sbar.w;
		} else if (space_for_bar) {
			r->w -= (sbar.w - scr->marginl[0] - scr->marginr[0]);
			if (!scr->alignside)
				r->x += (sbar.w - scr->marginl[0] - scr->marginr[0]);
		}

		sbar.x += scr->marginl[0];
		sbar.w -= (scr->marginl[0] + scr->marginr[0]);
		sbar.y = r->y + scr->marginu[0];
		sbar.h = r->h - scr->marginu[0] - scr->margind[0];

		if (draw_trough)
		{
			Color c;

			c = scr->troughcolor;
			if (scr->trough)
				backdrop_render(scr->trough, &sbar, bm);
			else if (c.a == 0xFF)
				bitmap_addbox(bm, &sbar, TO_RGB16(c));
			else if (c.a == 0xFE)
				bitmap_negbox(bm, &sbar, TO_RGB16(c));
			else if (c.a > 0x7F)
				bitmap_avgbox(bm, &sbar, TO_RGB16(c));
			else
				bitmap_fillbox(bm, &sbar, TO_RGB16(c));
		}

		if (draw_bar)
		{
			i = scr->lines;
			if(i < scr->showing)
				i = scr->showing;

			sbar.x += scr->marginl[1];
			sbar.w -= (scr->marginl[1] + scr->marginr[1]);
			sbar.y += scr->marginu[1];
			sbar.h -= (scr->marginu[1] + scr->margind[1]);

			sbar.y = sbar.y + scr->start * sbar.h / i;
			sbar.h = sbar.h * scr->showing / i;

			if (scr->bar)
				backdrop_render(scr->bar, &sbar, bm);
		}

		if (dirty)
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
	sc->marginl[0] = sc->marginr[0] = sc->marginl[1] = sc->marginr[1] = 1;
	sc->marginu[0] = sc->margind[0] = sc->marginu[1] = sc->margind[1] = 1;
	sc->style = TROUGH_ALWAYS | BAR_ALWAYS;

	p = (uint32 *)&sc->troughcolor;

	p[0] = 0xff181818;
	sc->bar = NULL;
	sc->trough = NULL;

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
	int i, n = attr & 0xf;
	uint32 l;

	switch(attr & 0xFF0)
	{
	case WATR_BACKDROP:
		if (n) {
			if (sc->trough)
				free(sc->trough);
			sc->trough = (BackDrop *)val;
		} else {
			if (sc->bar)
				free(sc->bar);
			sc->bar = (BackDrop *)val;
		}
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
		i = n & 1;
		switch ((n>>1) & 7)
		{
			case 1:
				sc->marginu[i] = sc->margind[i] = (int) val;
				break;
			case 2:
				sc->marginl[i] = (int) val;
				break;
			case 3:
				sc->marginr[i] = (int) val;
				break;
			case 4:
				sc->marginu[i] = (int) val;
				break;
			case 5:
				sc->margind[i] = (int) val;
				break;
			case 0:
			default:
				sc->marginl[i] = sc->marginr[i] = (int) val;
				break;
		}
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

