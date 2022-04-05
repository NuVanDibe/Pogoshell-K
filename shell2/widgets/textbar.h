#ifndef TEXTBAR_H
#define TEXTBAR_H

#include "../window.h"
#include "../backdrop.h"

#define WIDGET_TEXTBAR 0x1002
typedef struct
{
	Widget w;

	BackDrop *backdrop;
	char *text;
	uint16 align;
	uint16 margin;
	Font *font;
	Color textcolor;

} TextBar;

TextBar *textbar_new(Font *font, int texlen);
//void textbar_set_backdrop(TextBar *tb, BackDrop *bd);
//void textbar_set_color(TextBar *tb, Color *color);
void textbar_set_attribute(TextBar *tb, int attr, void *val);

#endif
