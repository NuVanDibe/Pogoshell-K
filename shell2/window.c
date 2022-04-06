#include "pogo.h"
#include "bitmap.h"
#include "window.h"

static Screen *currentScreen;
WidgetRenderFunc render_functions[8];

Color Black_Color = {0,0,0,0};
Color Blue_Color = {0,0,0xFF,0};
Color White_Color = {0xFF,0xFF,0xFF,0};
Color Gray_Color = {0x80,0x80,0x80,0};
Color Dark_Color = {0x18,0x18,0x18,0};

int widget_render(Widget *w, Rect *r, BitMap *bm)
{
	//int i, j;
	//fprintf(stderr, "Rendering widget 0x%p, 0x%04x, (%d, %d).\n", w, w->type, w->width, w->height);
	return render_functions[w->type & 0xFF](w, r, bm);
	//fprintf(stderr, "\n");
	//fprintf(stderr, "Done rendering widget 0x%p.\n", w);
	//return i;
}


Screen *screen_new()
{
	Screen *scr;
	scr = malloc(sizeof(Screen));
	scr->firstWindow = scr->lastWindow = NULL;
	scr->bitmap = bitmap_getscreen();
	currentScreen = scr;
	return scr;
}

void screen_redraw(Screen *scr)
{
	Window *w;
	Rect r;
	int rc = 0;

	r.x = 0;
	r.y = 0;
	r.w = scr->bitmap->width;
	r.h = scr->bitmap->height;
	
	if(!scr)
		scr = currentScreen;
	
	w = scr->firstWindow;
	while(w)
	{

		if(w->state & WIN_VISIBLE)
		{
			r.x = w->x;
			r.y = w->y;
			r.w = w->width;
			r.h = w->height;


			if(w->widget)
			{
				if(rc)
					w->widget->flags |= WFLG_REDRAW;

				rc += widget_render(w->widget, &r, w->parent->bitmap);
			}
		}

		w = w->next;
	}
}

/*
void window_redraw(Window *w)
{
	Rect r;

	r.x = w->x;
	r.y = w->y;
	r.w = w->width;
	r.h = w->height;

//	if(w->backing)
//		bitmap_blit(w->parent->bitmap, w->x, w->y, w->backing, 0, 0, w->width, w->height);

	if(w->widget)
		widget_render(w->widget, &r, w->parent->bitmap);
}
*/

void window_setup(Window *w, Screen *scr, int x, int y, int width, int height)
{
	w->x = x;
	w->y = y;
	w->height = height;
	w->width = width;
	w->parent = scr;
	w->widget = NULL;
	w->next = NULL;
	w->backing = NULL; //bitmap_new(width, height, BPP16);

	if(scr->lastWindow)
		scr->lastWindow->next = w;
	w->prev = scr->lastWindow;
	scr->lastWindow = w;
	if(!scr->firstWindow)
		scr->firstWindow = w;

	w->state = WIN_VISIBLE;
}

Window *window_new(Screen *scr, int x, int y, int width, int height)
{
	Window *w = malloc(sizeof(Window));
	window_setup(w, scr, x, y, width, height);
	return w;
}

// Check if given box overlaps window
/*
int window_checkbox(Window *win, int x, int y, int width, int height)
{
	if((win->x + win->width < x) || (x+width < win->x))
		return 0;

	if((win->y + win->height < y) || (y+height < win->y))
		return 0;

	return 1;
}

void window_move(Window *win, int x, int y)
{

	Window *w = win->parent->firstWindow;
	while(w)
	{
		if((w != win) && (window_checkbox(w, win->x, win->y, win->width, win->height) || 
		                  window_checkbox(w, x, y, win->width, win->height)) )

		w = w->next;
	}
	
	win->x = x;
	win->y = y;
}

void window_resize(Window *win,  int width, int height)
{
	Window *w = win->parent->firstWindow;
	int mw = width;
	int mh = height;

	if(win->width > mw) mw = win->width;
	if(win->height > mh) mh = win->height;

	while(w)
	{
		//if((w != win) && window_checkbox(w, win->x, win->y, mw, mh))
		//	w->state = 1;

		w = w->next;
	}
	
	win->width = width;
	win->height = height;
	//win->state = 1;
}

void window_clear(Window *w)
{
	if(w->backing)
		bitmap_clear(w->backing, 0xFFFF);
	//w->state = 1;
}

void window_text(Window *w, Font *font, int x, int y, char *text)
{
	uint16 *dst = w->backing->pixels + x + y * w->backing->width;
	font_text(font, text, dst, w->backing->width);
	//w->state = 1;
}
*/

void window_setwidget(Window *win, Widget *w)
{
	win->widget = w;
}

void window_dirty(Window *win)
{
	Window *w;
	Screen *scr = win->parent;
	w = scr->firstWindow;

	while(w && (w != win))
	{

		if(w->widget)
		{
			w->widget->flags |= WFLG_REDRAW;
			//fprintf(stderr, "window %p set dirty\n", w);
		}

		w = w->next;
	}

	win->widget->flags |= WFLG_REDRAW;
}

/*
TextWindow *textwindow_new(Screen *scr, int x, int y, int width, int height)
{
	TextWindow *tw = malloc(sizeof(TextWindow));
	window_setup(&tw->w, scr, x, y, width, height);
	return tw;
}


void textwindow_settext(TextWindow *tw, char *text)
{
	if(!tw->text || strlen(text) > strlen(tw->text))
	{
		free(tw->text);
		tw->text = malloc(strlen(text)+1);
	}
	strcpy(tw->text, text);

	tw->w.state = 1;
}

void textwindow_setfont(TextWindow *tw, Font *f)
{
	tw->font = f;
}

*/
