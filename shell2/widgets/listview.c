
#include "../bitmap.h"
#include "../window.h"
#include "../misc.h"
#include "listview.h"
#include "scrollbar.h"

// Fix start position
void fix_positions(ListView *lv, Rect *r)
{
	int s = lv->start;

	int showing = lv->showing; //(r->h - lv->marginy) / lv->lineh;

	if(lv->start > lv->lines - showing)
		lv->start = lv->lines - showing;

	while(lv->marked >= (lv->start + showing))
	{
		lv->start += 3;
	}
	while(lv->marked < lv->start)
	{
		lv->start -= 3;
	}

	if(lv->start > (lv->lines - showing))
		lv->start = (lv->lines - showing);

	if(lv->start < 0) lv->start = 0;

	if(s != lv->start)
		lv->w.flags |= WFLG_REDRAW;

}

static void draw_gradient(ListView *lv, Rect *r, uint16 *dst, BitMap *bm)
{
	int i, j, l;
	int dh;
	uint16 gdcol;
	Color col, *colptr;

	if (lv->gradientstyle == SIMPLE) {
		dh = l = 0;
		for (i = 0; i < r->h; i++)
		{
			colptr = lv->associatecolors[l];
			col = (colptr) ? *colptr : lv->textcolor[0];
			gdcol = TO_RGB16(col);
			for (j = 0; j < lv->gradientwidth; j++)
				*dst++ = gdcol;
			dh += lv->lines;
			while (dh >= r->h) {
				l++;
				dh -= r->h;
			}
			dst += (bm->width - lv->gradientwidth);
		}
	} else if (lv->gradientstyle == GRADIENT) {
		Color col2;
		int x;
		uint16 c0, c1, c2, d0, d1, d2;

		dh = 0;
		colptr = lv->associatecolors[0];
		col = (colptr) ? *colptr : lv->textcolor[0];

		for (i = 0; i < lv->lines; i++)
		{
			x = 0;
			dh += r->h;
			while (dh >= lv->lines) {
				x++;
				dh -= (lv->lines);
			}
			if (x) {
				colptr = lv->associatecolors[i];
				col2 = (colptr) ? *colptr : lv->textcolor[0];
				c0 = col.b << 7;
				c1 = col.g << 2;
				c2 = col.r << 2;
				d0 = ((col2.b - col.b) << 7) / x;
				d1 = ((col2.g - col.g) << 2) / x;
				d2 = ((col2.r - col.r) << 2) / x;

				while (x--) {
					gdcol = (c0 & 0x7c00) | (c1 & 0x03e0) | ((c2 >> 5) & 0x1f);
					c0 += d0;
					c1 += d1;
					c2 += d2;
					for (j = 0; j < lv->gradientwidth; j++)
						*dst++ = gdcol;
					dst += (bm->width - lv->gradientwidth);
				}
				col = col2;
			}
		}
	}
}

int listview_render(ListView *lv, Rect *org_r, BitMap *bm)
{
	//const char col[3] = { 0xFF, 0x00, 0x00};
	//const char bcol[3] = { 0xC0, 0xC0, 0xC0};
	Font *f = lv->typeface->font;
	Rect r2;
	int i, j, l, y, maxi;
	uint16 bdcol;
	int drawwidth, freewidth;
	Color c, *cp;
	
	int lineh = lv->lineh;
	int fonty = (lv->lineh - font_height(f)) / 2;

	uint16 *d, *dst;

	Rect tr = *org_r, cr = *org_r;
	Rect *r = &cr;

	BitMap *sbm = NULL;
	
	if(lv->backdrop)
	{
		backdrop_render(lv->backdrop, &tr, NULL); // to fix tr, if necessary
		sbm = lv->backdrop->bitmap;
		bdcol = TO_RGB16(lv->backdrop->color[2]);
	}
	else
		bdcol = 0x6318; //COL16(bcol);

	//r->x += 4;

	lv->showing = (tr.h - lv->marginy) / lv->lineh;

	fix_positions(lv, &tr);

	if(lv->w.flags & WFLG_REDRAW)
		lv->dirty = 0xFF;

	lv->w.flags &= ~WFLG_REDRAW;

	if(!lv->dirty)
		return 0;

	y = 0;

	if(lv->dirty == 0xFF)
	{
		if (lv->scrollbar) {
			scrollbar_set_lines(lv->scrollbar, lv->lines);
			scrollbar_set_start(lv->scrollbar, lv->start);
			scrollbar_set_showing(lv->scrollbar, lv->showing);
			scrollbar_set_dirty(lv->scrollbar);
		}
		if(lv->backdrop)
			backdrop_render(lv->backdrop, r, bm);
		/*if(sbm)
			bitmap_blit(bm, r->x, r->y, sbm, 0, 0, r->w, r->h);*/
		else
			bitmap_fillbox(bm, r, bdcol);
		//fprintf(stderr, "FULL!\n");
	} else if (lv->backdrop)
		backdrop_render(lv->backdrop, r, NULL); // to fix r, if necessary

	r->y += lv->marginy;

	if (lv->columns == 2 && lv->gradientstyle) {
		if (lv->gradientwidth &&
			(lv->gradientalign == ALIGN_LEFT || lv->gradientalign == ALIGN_RIGHT))
		{	
			lv->colwidth[0] -= lv->gradientwidth/2;
			lv->colwidth[1] -= (lv->gradientwidth - lv->gradientwidth/2);
			dst = (uint16 *) bm->pixels + r->y * bm->width + r->x;
			if (lv->gradientalign == ALIGN_LEFT)
				r->x += lv->gradientwidth;
			else
				dst += r->w - lv->gradientwidth;
			r->w -= lv->gradientwidth;
			draw_gradient(lv, r, dst, bm);
		}
	}

	if (lv->scrollbar)
		scrollbar_render(lv->scrollbar, r, bm);

	if (lv->columns == 2) {
		if (lv->scrollbar) {
			lv->colwidth[0] += lv->scrollbar->addleft;
			lv->colwidth[1] += lv->scrollbar->addright;
		}
		if (lv->gradientstyle && lv->gradientwidth && lv->gradientalign == ALIGN_CENTER)	{	
			lv->colwidth[0] -= lv->gradientwidth/2;
			lv->colwidth[1] -= (lv->gradientwidth - lv->gradientwidth/2);
			dst = (uint16 *) bm->pixels + r->y * bm->width + r->x;
			if (lv->scrollbar && !(lv->scrollbar->alignside))
				r->x += lv->gradientwidth;
			else
				dst += r->w - lv->gradientwidth;
			r->w -= lv->gradientwidth;
			draw_gradient(lv, r, dst, bm);
		}
	}

	if(!lv->lines) {
		if (lv->columns == 2) {
			if (lv->gradientstyle && lv->gradientwidth &&
				(lv->gradientalign == ALIGN_LEFT || lv->gradientalign == ALIGN_RIGHT || lv->gradientalign == ALIGN_CENTER)) {
				lv->colwidth[0] += lv->gradientwidth/2;
				lv->colwidth[1] += (lv->gradientwidth - lv->gradientwidth/2);
			}
			if (lv->scrollbar) {
				lv->colwidth[0] -= lv->scrollbar->addleft;
				lv->colwidth[1] -= lv->scrollbar->addright;
			}
		}
		lv->dirty = 0;
	    for (i = 0; i < MAPSLOTCOUNT; i++)
        	lv->redrawmap[i] = 0;
		return 1;
	}

	dst = (uint16 *)bm->pixels + (r->x + lv->iconw + lv->marginx) + (r->y + fonty) * bm->width ;

//	fprintf(stderr, "%d / %d = %d\n", r->h, lv->lineh, lv->showing);

	maxi = lv->start + lv->showing;
	if(maxi > lv->lines)
		maxi = lv->lines;

	r2.x = r->x;
	r2.y = r->y;
	r2.w = r->w;
	r2.h = lineh;

	freewidth = 0;
	for(j=0; j<lv->columns; j++)
		freewidth += lv->colwidth[j];

	font_setshadowoutline(TO_RGB16(lv->typeface->shadow),TO_RGB16(lv->typeface->outline));
	//font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);
	for(i=lv->start; i<maxi; i++)
	{
		if(lv->dirty == 0xFF || lv->redrawmap[i>>5]&(1<<(i&31)))
		{
			r2.y = r->y + y;
			//fprintf(stderr, "Redrawing %d\n", i);

			if (lv->dirty != 0xFF) {
				if (lv->backdrop)
					backdrop_subrender(lv->backdrop, &tr, &r2, bm);
				else
					bitmap_fillbox(bm, &r2, bdcol);
			}

			d = dst;

			cp = lv->associatecolors[i];
			if(i == lv->marked)
			{
				c = lv->textcolor[3];
				if(c.a == 0xFF)
					bitmap_addbox(bm, &r2, TO_RGB16(c));
				else if(c.a == 0xFE)
					bitmap_negbox(bm, &r2, TO_RGB16(c));
				else if (c.a > 0x7a) {
					if (c.a == 0x80 || c.a > 0x85)
						bitmap_avgbox(bm, &r2, TO_RGB16(c));
					else if (c.a < 0x80)
						bitmap_addshiftbox(bm, &r2, TO_RGB16(c), 0x80 - c.a);
					else
						bitmap_avgshiftbox(bm, &r2, TO_RGB16(c), c.a - 0x80);
				} else
					bitmap_fillbox(bm, &r2, TO_RGB16(c));
				c = (cp && (lv->associatestyle & COLORHIGHLIGHT)) ? *cp : lv->textcolor[2];
				font_setcolor(TO_RGB16(c), TO_RGB16(lv->textcolor[3]));	
			} else {
				c = (cp && (lv->associatestyle & COLORTEXT)) ? *cp : lv->textcolor[0];
				font_setcolor(TO_RGB16(c), 0x0000);	
			}
			for(j=0; j<lv->columns; j++)
			{
				if (lv->colwidth[j]) {
					drawwidth = (i == lv->marked || (lv->truncatestyle == FULL)) ? freewidth : lv->colwidth[j];
					if (lv->colalign[j] == ALIGN_LEFT) {
						l = font_text_truncate(f, lv->texts[j][i], d, bm->width, drawwidth);
					} else {
						l = font_text_truncate(f, lv->texts[j][i], NULL, bm->width, drawwidth);
						l = (lv->colalign[j] == ALIGN_RIGHT) ? lv->colwidth[j]-l : ((lv->colwidth[j]-l)>>1);
						l = font_text_truncate(f, lv->texts[j][i], d + l, bm->width, drawwidth);
					}
				}
				d += lv->colwidth[j]; 
			}
			//font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);
			if(lv->icons[i])
				bitmap_blit(bm, r->x + lv->marginx, r->y + y, lv->icons[i], 0, 0, lv->icons[i]->width, lv->icons[i]->height);
		}
		dst += (bm->width * lineh);
		y += lineh;
	}

	if (lv->columns == 2) {
		if (lv->gradientwidth &&
			(lv->gradientalign == ALIGN_LEFT || lv->gradientalign == ALIGN_RIGHT || lv->gradientalign == ALIGN_CENTER)) {
			lv->colwidth[0] += lv->gradientwidth/2;
			lv->colwidth[1] += (lv->gradientwidth - lv->gradientwidth/2);
		}
		if (lv->scrollbar) {
			lv->colwidth[0] -= lv->scrollbar->addleft;
			lv->colwidth[1] -= lv->scrollbar->addright;
		}
	}

	lv->dirty = 0;
    for (i = 0; i < MAPSLOTCOUNT; i++)
        lv->redrawmap[i] = 0;

	return 1;
}

void listview_set_attribute(ListView *lv, int attr, void *val)
{
	Color c;
	Font *f;
	int n = attr & 0xf;
	uint32 l;

	switch(attr & 0xFFF0)
	{
	case WATR_BACKDROP:
		if(lv->backdrop)
			free(lv->backdrop);

		lv->backdrop = (BackDrop *)val;
		lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
		lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;
		lv->w.height += lv->backdrop->border*2;
		lv->w.width += lv->backdrop->border*2;
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_COLWIDTH:
		if (n < 8)
			lv->colwidth[n] = (int)val;
		else
			lv->gradientwidth = (int)val;
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_RGB:
		l = (int)val;
		c.r = (l>>16) & 0xff;
		c.g = (l>>8) & 0xff;
		c.b = l & 0xff;
		c.a = l>>24;

		//fprintf(stderr, "RGBCOLOR SET %06x\n", l);

		lv->textcolor[n] = c;
		lv->w.flags |= WFLG_REDRAW;

		break;

	case WATR_COLOR:
		lv->textcolor[n] = *((Color *)val);

		//fprintf(stderr, "TEXTCOLOR SET\n");

		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_FONT:
		if (!lv->typeface->global) {
			if (lv->typeface->font)
				free(lv->typeface->font);
			free(lv->typeface);
		}
		
		f = (Font *)val;
		lv->typeface = typeface_new(f, 0);

		if (font_height(f) > lv->lineh)
			lv->lineh = font_height(f) + 1;
		lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
		if (lv->backdrop)
			lv->w.height += lv->backdrop->border*2;
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_SCROLLBAR:
		if(lv->scrollbar)
			free(lv->scrollbar);
		lv->scrollbar = (Scrollbar *)val;
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_STYLE:
		switch (n)
		{
			case ASSOCIATESTYLE:
				lv->associatestyle = (int)val;
				//fprintf(stderr, "asociatestyle: %d\n", lv->associatestyle);
				break;
			case TRUNCATESTYLE:
				lv->truncatestyle = (int)val;
				break;
			case GRADIENTSTYLE:
			default:
				lv->gradientstyle = (int)val;
				break;
		}
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_MARGIN:
		if((attr & 0xF) == 0)
			lv->marginx = (int)val;
		else
		if((attr & 0xF) == 1)
			lv->marginy = (int)val;
		lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
		lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;
		if (lv->backdrop) {
			lv->w.height += lv->backdrop->border*2;
			lv->w.width += lv->backdrop->border*2;
		}
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_ALIGN:
		if (n < 8)
			lv->colalign[n] = (int)val;
		else
			lv->gradientalign = (int)val;
		lv->w.flags |= WFLG_REDRAW;
		break;
	case WATR_TYPEFACE:
		if (!lv->typeface->global)
			free(lv->typeface);
		lv->typeface = (Typeface *)val;
		lv->w.flags |= WFLG_REDRAW;
		break;
	//case WATR_NAME:
	//	strcpy(lv->w.name, (char *)val);
	//	break;
	}

}


ListView *listview_new(int columns, int maxlines, Font *font)
{
	return listview_new_typeface(columns, maxlines, typeface_new(font, 0));
}

ListView *listview_new_typeface(int columns, int maxlines, Typeface *tf)
{
	int i;
	uint32 *p;
	ListView *lv = malloc(sizeof(ListView)); // + (maxlines + maxlines * columns) * 4);

	render_functions[WIDGET_LISTVIEW & 0xFFF] = (WidgetRenderFunc)listview_render;

	lv->w.height = 0;
	lv->w.width = 0;
	lv->marginx = 0;
	lv->marginy = 0;

	lv->w.type = WIDGET_LISTVIEW;
	lv->w.flags = WFLG_REDRAW;

	lv->typeface = tf;

	if (tf->font)
		lv->lineh = font_height(tf->font) + 1;
	else
		lv->lineh = 0;

	lv->linew = 0;

	p = (uint32 *)&lv->textcolor[0];

	p[0] = 0x01000000;
	p[2] = 0x00FFFFFF;
	p[3] = 0x00FF0000;

	lv->iconw = 0;

	lv->icons = pmalloc(sizeof(BitMap *) * maxlines);
	lv->associatecolors = pmalloc(sizeof(Color *) * maxlines);
	//memset(lv->icons, 0, sizeof(BitMap *) * maxlines);

	for(i=0; i<columns; i++)
	{
		lv->texts[i] = (char **)pmalloc(sizeof(char *) * maxlines);
		//memset(lv->texts[i], 0, sizeof(char *) * maxlines);
		lv->colwidth[i] = 1;
		lv->colalign[i] = ALIGN_LEFT;
	}
	//for(i=0; i<maxlines; i++)
	//{
	//	lv->texts[i] = ptr;
	//	ptr += ;
	//}

	lv->backdrop = NULL;

	lv->marked = -1;
	lv->start = 0;
	lv->lines = 0;
	//lv->showing = 9;

	lv->columns = columns;

	lv->associatestyle = COLORTEXT;
	lv->truncatestyle = TRUNCATE;

	lv->dirty = 0xFF;

	lv->scrollbar = NULL;
	//lv->scrollbar = backdrop_new(STYLE_BEVEL);
	//backdrop_set_border(lv->scrollbar, 1);

	lv->gradientwidth = 2;
	lv->gradientalign = ALIGN_RIGHT;
	lv->gradientstyle = SIMPLE; 

	return lv;

}

void listview_clear(ListView *lv)
{
	if(!lv->lines)
		return;

	lv->lines = 0;
	lv->marked = -1;
	lv->start = 0;
	lv->lineh = font_height(lv->typeface->font) + 1;
	lv->iconw = 0;
	lv->linew = 0;
	lv->w.height = lv->marginy * 2;
	lv->w.width = lv->marginx * 2;
	if (lv->backdrop) {
		lv->w.height += lv->backdrop->border*2;
		lv->w.width += lv->backdrop->border*2;
	}
	lv->w.flags |= WFLG_REDRAW;
}

void listview_set_dirty(ListView *lv)
{
	lv->w.flags |= WFLG_REDRAW;
}

void listview_set_start(ListView *lv, int i)
{

	if(i >= lv->lines || i == lv->start || i < 0)
		return;

	lv->start = i;
	lv->w.flags |= WFLG_REDRAW;
}

void listview_set_marked(ListView *lv, int i)
{

	if(i >= lv->lines || i < 0)
		return;

	if(i == lv->marked)
		return;

	lv->dirty = 1;

	if (lv->marked != -1)
	   lv->redrawmap[(lv->marked)>>5] |= 1<<(lv->marked&31);
	
	lv->redrawmap[i>>5] |= 1<<(i&31);

	lv->marked = i;
}

void listview_setline(ListView *lv, int index, Color *cl, BitMap *bm, ...)
{
	int i, w = 0;
	va_list vl;
	va_start(vl, bm);

	lv->associatecolors[index] = cl;
	lv->icons[index] = bm;

	if(bm)
	{
		if(bm->height >= lv->lineh)
			lv->lineh = bm->height + 1;

		if((bm->width + 2) > lv->iconw)
			lv->iconw = bm->width + 2;
	}

	w = 0;
	for(i=0; i<lv->columns; i++)
	{
		lv->texts[i][index] = va_arg(vl, char *);
		w += font_text(lv->typeface->font, lv->texts[i][index], NULL, 240);
	}


	if(w > lv->linew)
		lv->linew = w;

	//lv->w.flags |= WFLG_REDRAW;
	//lv->lines++;
	lv->dirty = 1;
	lv->redrawmap[index>>5] |= 1<<(index&31);

	lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
	lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;

	if (lv->backdrop) {
		lv->w.height += lv->backdrop->border*2;
		lv->w.width += lv->backdrop->border*2;
	}

	va_end(vl);
}

void listview_addline(ListView *lv, Color *cl, BitMap *bm, ...)
{
	int i, w = 0;
	va_list vl;
	va_start(vl, bm);


	lv->associatecolors[lv->lines] = cl;
	lv->icons[lv->lines] = bm;

	if(bm)
	{
		if(bm->height >= lv->lineh)
			lv->lineh = bm->height + 1;

		if((bm->width + 2) > lv->iconw)
			lv->iconw = bm->width + 2;
	}

	w = 0;
	for(i=0; i<lv->columns; i++)
	{
		lv->texts[i][lv->lines] = va_arg(vl, char *);
		w += font_text(lv->typeface->font, lv->texts[i][lv->lines], NULL, 240);
	}


	if(w > lv->linew)
		lv->linew = w;

	lv->w.flags |= WFLG_REDRAW;

	lv->lines++;

	lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
	lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;

	if (lv->backdrop) {
		lv->w.height += lv->backdrop->border*2;
		lv->w.width += lv->backdrop->border*2;
	}

	va_end(vl);

}
