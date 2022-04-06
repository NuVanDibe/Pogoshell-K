#ifndef TEXTBAR_H
#define TEXTBAR_H

#include "../window.h"
#include "../backdrop.h"
#include "typeface.h"

#define WIDGET_TEXTBAR 0x1002
typedef struct
{
	Widget w;

	BackDrop *backdrop;
	char *text;
	uint16 align;
	uint16 margin;
	Typeface *typeface;
	Color textcolor;

} TextBar;

TextBar *textbar_new(Font *font, int texlen);
void textbar_set_attribute(TextBar *tb, int attr, void *val);

#endif
