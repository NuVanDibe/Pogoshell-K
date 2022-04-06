
#include "../bitmap.h"
#include "../window.h"
#include "tricontainer.h"


int tricontainer_render(TriContainer *tri, Rect *r, BitMap *bm)
{
	Rect tmpr = *r;
	int h0 = 0;
	int h2 = 0;
	int rc = 0;

	int redraw = tri->w.flags & WFLG_REDRAW;
	tri->w.flags &= ~WFLG_REDRAW;

	if(tri->children[0])
	{	
		h0 = tmpr.h = tri->children[0]->height;

		tri->children[0]->flags |= redraw;
		rc += widget_render(tri->children[0], &tmpr, bm);
	}

	if(tri->children[2])
	{
		h2 = tmpr.h = tri->children[2]->height;
		tmpr.y = r->y + r->h - h2;

		tri->children[2]->flags |= redraw;
		rc += widget_render(tri->children[2], &tmpr, bm);
	}

	if(tri->children[1])
	{
		tmpr.h = r->h - h2 - h0;
		tmpr.y = r->y + h0;

		tri->children[1]->flags |= redraw;
		rc += widget_render(tri->children[1], &tmpr, bm);
	}

	return rc;
}

TriContainer *tricontainer_new()
{
	int i;

	TriContainer *tc = malloc(sizeof(TriContainer));

	render_functions[WIDGET_TRICONTAINER & 0xFFF] = (WidgetRenderFunc)tricontainer_render;

	tc->w.type = WIDGET_TRICONTAINER;
	tc->w.height = 0;
	tc->w.width = 0;

	tc->w.flags = WFLG_REDRAW;

	for(i=0; i<3; i++)
		tc->children[i] = NULL;

	return tc;

}

void tricontainer_set_attribute(TriContainer *tc, int attr, void *val)
{
	switch(attr & 0xFF0)
	{
	case WATR_CHILD:
		tc->children[attr & 0xF] = (Widget *)val;
		tc->w.flags |= WFLG_REDRAW;
		((Widget *)val)->flags |= WFLG_REDRAW;
		break;
	//case WATR_NAME:
	//	strcpy(tc->w.name, (char *)val);
	//	break;
	}
}

void tricontainer_setwidget(TriContainer *tri, int i, Widget *w)
{
	//POGO_ASSERT(tri->w.type == WIDGET_TRICONTAINER);

	tri->children[i] = w;
	tri->w.flags |= WFLG_REDRAW;
}
