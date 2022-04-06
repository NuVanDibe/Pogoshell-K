#ifndef WINDOW_H
#define WINDOW_H

#include "bitmap.h"


#define TO_RGB16(c) ( ((c.b << 7) & 0x7C00) | ((c.g << 2) & 0x03E0) | (c.r >> 3) )

extern Color Black_Color;
extern Color Blue_Color;
extern Color White_Color;
extern Color Gray_Color;
extern Color Dark_Color;

typedef struct
{
	uint16 type;
	uint16 width;
	uint16 height;
	uint16 flags;
	//char name[16];

} Widget;

#define WFLG_REDRAW 1

// Widget attributes
enum
{
	WATR_BACKDROP = 0x10,
	WATR_COLOR = 0x20,
	WATR_COLWIDTH = 0x30,
	WATR_TEXT = 0x40,
	WATR_ALIGN = 0x50,
	WATR_STYLE = 0x60,
	WATR_FONT = 0x70,
	WATR_BITMAP = 0x80,
	WATR_BORDER = 0x90,
	WATR_CHILD = 0xA0,
	WATR_NAME = 0xB0,
	WATR_RGB = 0xC0,
	WATR_SCROLLBAR = 0xD0,
	WATR_MARGIN = 0xE0,
	WATR_HEIGHT = 0xF0,
	WATR_TYPEFACE = 0x100,
};

#define POGO_ASSERT(x) if(!(x)) { dprint(#x); exit(0); }

enum {ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};


struct _Window;

typedef int (*WidgetRenderFunc)(Widget *, Rect *, BitMap *);

extern WidgetRenderFunc render_functions[8];

enum {WIN_VISIBLE = 1};

typedef struct
{
	struct _Window *firstWindow;
	struct _Window *lastWindow;
	BitMap *bitmap;

} Screen;

typedef struct _Window
{
	uint16 width;
	uint16 height;

	uint16 x;
	uint16 y;
	BitMap *backing;
	Screen *parent;
	struct _Window *next;
	struct _Window *prev;

	Widget *widget;

	uint16 state;

} Window;

Screen *screen_new();
void screen_redraw(Screen *scr);

Window *window_new(Screen *screen, int x, int y, int width, int height);
void window_move(Window *win, int x, int y);
void window_resize(Window *win, int w, int h);
//void window_redraw(Window *w);
void window_clear(Window *w);
void window_setwidget(Window *win, Widget *w);

int widget_render(Widget *w, Rect *r, BitMap *bm);

void window_dirty(Window *win);
#define window_hide(w) {(w)->state &= ~WIN_VISIBLE; window_dirty(w);}
#define window_show(w) ((w)->state |= WIN_VISIBLE)

#endif
