
#include "../bitmap.h"
#include "../window.h"
#include "listview.h"

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

int listview_render(ListView *lv, Rect *r, BitMap *bm)
{
	//const char col[3] = { 0xFF, 0x00, 0x00};
	//const char bcol[3] = { 0xC0, 0xC0, 0xC0};
	Rect r2;
	int i, j, l, y, maxi;
	uint16 bdcol;
	
	int lineh = lv->lineh;
	int fonty = (lv->lineh - lv->font->height) / 2;

	uint16 *d, *dst;

	Rect sbar = *r;

	BitMap *sbm = NULL;
	
	if(lv->backdrop)
	{
		sbm = lv->backdrop->bitmap;
		bdcol = TO_RGB16(lv->backdrop->color[2]);
	}
	else
		bdcol = 0x6318; //COL16(bcol);



	if(lv->w.flags & WFLG_REDRAW)
		lv->dirty = 0xFF;

	lv->w.flags &= ~WFLG_REDRAW;

	if(!lv->dirty)
		return 0;

	y = 0;

	if(lv->dirty == 0xFF)
	{
		if(sbm)
			bitmap_blit(bm, r->x, r->y, sbm, 0, 0, r->w, r->h);
		else
			bitmap_fillbox(bm, r, bdcol);
		//fprintf(stderr, "FULL!\n");
	}


	if(!lv->lines)
		return 1;


	//r->x += 4;
	r->y += lv->marginy;	

	lv->showing = (r->h - lv->marginy) / lv->lineh;

	fix_positions(lv, r);


	dst = (uint16 *)bm->pixels + (r->x + lv->iconw + lv->marginx) + (r->y + fonty) * bm->width ;


//	fprintf(stderr, "%d / %d = %d\n", r->h, lv->lineh, lv->showing);

	maxi = lv->start + lv->showing;
	if(maxi > lv->lines)
		maxi = lv->lines;

	r2.x = r->x;
	r2.y = r->y;
	r2.w = r->w;
	if(lv->scrollbar)
		r2.w -= 10;
	r2.h = lineh;

	for(i=lv->start; i<maxi; i++)
	{
		if(lv->dirty == 0xFF || lv->redrawmap[i>>5]&(1<<(i&31)))
		{
			r2.y = r->y + y;
			//fprintf(stderr, "Redrawing %d\n", i);

			if (lv->dirty != 0xFF) {
				if(sbm)
					bitmap_blit(bm, r2.x, r2.y, sbm, 0, (y+lv->marginy) % sbm->height, r2.w, lineh);
				else
				{
					bitmap_fillbox(bm, &r2, bdcol);
				}
			}

			d = dst;

			for(j=0; j<lv->columns; j++)
			{
				if(j == 0 && i == lv->marked)
				{
					if(lv->textcolor[3].a > 0x7F)
						bitmap_addbox(bm, &r2, TO_RGB16(lv->textcolor[3]));
					else
						bitmap_fillbox(bm, &r2, TO_RGB16(lv->textcolor[3]));
					font_setcolor(TO_RGB16(lv->textcolor[2]), TO_RGB16(lv->textcolor[3]));
					l = font_text(lv->font, lv->texts[j][i], d, bm->width);

					if(lv->textcolor[0].a != 0x01)
						font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);
					else
						font_setcolor(0, 0);
				}
				else {
					if (lv->colwidth[j])
						l = font_text(lv->font, lv->texts[j][i], d, bm->width);
				}
				d += lv->colwidth[j]; 
			}

			if(lv->icons[i])
				bitmap_blit(bm, r->x + lv->marginx, r->y + y, lv->icons[i], 0, 0, lv->icons[i]->width, lv->icons[i]->height);
		}
		dst += (bm->width * lineh);
		y += lineh;
	}

	if(lv->scrollbar && lv->dirty == 0xFF)
	{
		sbar.x += (sbar.w - 9);
		sbar.w = 8;
		sbar.y += 1;
		sbar.h -= 2;

		bitmap_addbox(bm, &sbar, 0x0C63);

		i = lv->lines;
		if(i < lv->showing)
			i = lv->showing;

		sbar.w-=2;
		sbar.x++;
		sbar.y = sbar.y + lv->start * sbar.h / i;

		sbar.h = sbar.h * lv->showing / i;

		backdrop_render(lv->scrollbar, &sbar, bm);
	}

	lv->dirty = 0;
    for (i = 0; i < MAPSLOTCOUNT; i++)
        lv->redrawmap[i] = 0;

	return 1;
}

void listview_set_attribute(ListView *lv, int attr, void *val)
{
	Color c;
	int i;
	int n = attr & 0xf;
	uint32 l;

	switch(attr & 0xFF0)
	{
	case WATR_BACKDROP:
		if(lv->backdrop)
			free(lv->backdrop);

		lv->backdrop = (BackDrop *)val;
		lv->w.flags = WFLG_REDRAW;
		break;
	case WATR_COLWIDTH:
		lv->colwidth[n] = (int)val;
		lv->w.flags = WFLG_REDRAW;
		break;
	case WATR_RGB:
		l = (int)val;
		c.r = (l>>16) & 0xff;
		c.g = (l>>8) & 0xff;
		c.b = l & 0xff;
		c.a = l>>24;

		//fprintf(stderr, "RGBCOLOR SET %06x\n", l);


		lv->textcolor[n] = c;
		if(!n && lv->font)
				font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);

		lv->w.flags = WFLG_REDRAW;

		break;

	case WATR_COLOR:
		lv->textcolor[n] = *((Color *)val);

		if(!n)
		{
			lv->textcolor[0].a = 0;
			if(lv->font)
				font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);
		}

		//fprintf(stderr, "TEXTCOLOR SET\n");

		lv->w.flags = WFLG_REDRAW;
		break;
	case WATR_FONT:
//		fprintf(stderr, "font %p replaces %p\n", val, lv->font);
		if(lv->font)
			free(lv->font);
		lv->font = (Font *)val;
		if(lv->font->height >= lv->lineh)
			lv->lineh = lv->font->height + 1;
		lv->w.flags = WFLG_REDRAW;
		lv->w.height = lv->lineh * lv->lines + lv->marginy;
		if(lv->textcolor[0].a != 0x01);
			font_setcolor(TO_RGB16(lv->textcolor[0]), 0x0000);
		break;
	case WATR_SCROLLBAR:
		if(lv->scrollbar)
			free(lv->scrollbar);
		lv->scrollbar = (BackDrop *)val;
		lv->w.flags = WFLG_REDRAW;
		break;
	case WATR_MARGIN:
		if((attr & 0xF) == 0)
			lv->marginx = (int)val;
		else
		if((attr & 0xF) == 1)
			lv->marginy = (int)val;
		lv->w.height = lv->lineh * lv->lines + lv->marginy;
		break;
	//case WATR_NAME:
	//	strcpy(lv->w.name, (char *)val);
	//	break;
	}

}


ListView *listview_new(int columns, int maxlines, Font *font)
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
	lv->font = font;

	if(font)
		lv->lineh = font->height + 1;
	else
		lv->lineh = 0;

	lv->linew = 0;
	lv->textcolor[0].a = 1;

	p = (int *)&lv->textcolor[2];

	p[0] = 0x00FFFFFF;
	p[1] = 0x00FF0000;

	lv->iconw = 0;

	lv->icons = malloc(sizeof(BitMap *) * maxlines);
	memset(lv->icons, 0, sizeof(BitMap *) * maxlines);

	for(i=0; i<columns; i++)
	{
		lv->texts[i] = (char **)malloc(sizeof(char *) * maxlines);
		memset(lv->texts[i], 0, sizeof(char *) * maxlines);
		lv->colwidth[i] = 1;
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

	lv->dirty = 0xFF;

	lv->scrollbar = NULL;
	//lv->scrollbar = backdrop_new(STYLE_BEVEL);
	//backdrop_set_border(lv->scrollbar, 1);

	return lv;

}

void listview_clear(ListView *lv)
{
	if(!lv->lines)
		return;

	lv->lines = 0;
	lv->marked = -1;
	lv->start = 0;
	lv->lineh = lv->font->height + 1;
	lv->iconw = 0;
	lv->linew = 0;
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

void listview_setline(ListView *lv, int index, BitMap *bm, ...)
{
	int i, w = 0;
	va_list vl;
	va_start(vl, bm);

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
		w += font_text(lv->font, lv->texts[i][index], NULL, 240);
	}


	if(w > lv->linew)
		lv->linew = w;

	//lv->w.flags |= WFLG_REDRAW;
	//lv->lines++;
	lv->dirty = 1;
	lv->redrawmap[index>>5] |= 1<<(index&31);

	lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
	lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;

	va_end(vl);
}

void listview_addline(ListView *lv, BitMap *bm, ...)
{
	int i, w = 0;
	va_list vl;
	va_start(vl, bm);


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
		w += font_text(lv->font, lv->texts[i][lv->lines], NULL, 240);
	}


	if(w > lv->linew)
		lv->linew = w;

	lv->w.flags |= WFLG_REDRAW;

	lv->lines++;

	lv->w.height = lv->lineh * lv->lines + lv->marginy * 2;
	lv->w.width = lv->iconw + lv->linew + lv->marginx * 2;

	va_end(vl);

}
