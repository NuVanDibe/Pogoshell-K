#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "../window.h"
#include "../backdrop.h"

enum {
	TROUGH_NEVER = 0,
	TROUGH_PARTIAL = 1,
	TROUGH_ALWAYS = 2,
	BAR_NEVER = 0,
	BAR_PARTIAL = 4,
	BAR_ALWAYS = 8
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
	uint16 marginx[2];
	uint16 marginy[2];
	uint16 alignside;

	Color troughcolor;

} Scrollbar;

Scrollbar *scrollbar_new();
void scrollbar_set_attribute(Scrollbar *sc, int attr, void *val);
void scrollbar_set_start(Scrollbar *sc, int start);
void scrollbar_set_showing(Scrollbar *sc, unsigned int showing);
void scrollbar_set_lines(Scrollbar *sc, unsigned int lines);
void scrollbar_set_dirty(Scrollbar *sc);

#endif
