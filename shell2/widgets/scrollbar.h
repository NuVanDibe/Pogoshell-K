#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "../window.h"
#include "../backdrop.h"

enum {
	TROUGH_NEVER = 0,
	TROUGH_MINIMALLEFT = 1,
	TROUGH_MINIMALRIGHT = 2,
	TROUGH_MINIMALBOTH = 3,
	TROUGH_PARTIAL = 4,
	TROUGH_ALWAYS = 5,
	BAR_NEVER = 0,
	BAR_MINIMALLEFT = 8,
	BAR_MINIMALRIGHT = 16,
	BAR_MINIMALBOTH = 24,
	BAR_PARTIAL = 32,
	BAR_ALWAYS = 40
};

#define WIDGET_SCROLLBAR 0x1005
typedef struct
{
	Widget w;

	int16 start;
	uint16 lines;
	uint16 showing;
	
	uint16 style;
	BackDrop *bar;
	BackDrop *trough;
	BackDrop *troughtopbutton;
	BackDrop *troughbottombutton;
	uint16 marginl[2];
	uint16 marginr[2];
	uint16 marginu[2];
	uint16 margind[2];
	uint16 alignside;
	uint16 addleft;
	uint16 addright;

	Color troughcolor;

} Scrollbar;

enum { BARBACKDROP = 0, TROUGHBACKDROP = 1, TROUGHTOPBUTTONBACKDROP = 2, TROUGHBOTTOMBUTTONBACKDROP = 3};

int scrollbar_render(Scrollbar *scr, Rect *r, BitMap *bm);
Scrollbar *scrollbar_new();
void scrollbar_set_attribute(Scrollbar *sc, int attr, void *val);
void scrollbar_set_start(Scrollbar *sc, int start);
void scrollbar_set_showing(Scrollbar *sc, unsigned int showing);
void scrollbar_set_lines(Scrollbar *sc, unsigned int lines);
void scrollbar_set_dirty(Scrollbar *sc);

#endif
