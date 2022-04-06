#ifndef TEXTFLOW_H
#define TEXTFLOW_H

#include "../window.h"
#include "../backdrop.h"

#define WIDGET_TEXTFLOW 0x1004
typedef struct
{
	Widget w;

	BackDrop *backdrop;

	char *text;
	char *textline[20];
	uint16 textlength[20];
	uint16 numlines;
	uint16 align;
	Font *font;
	Color textcolor[4];

} TextFlow;

TextFlow *textflow_new(Font *font, int texlen);
void textflow_set_attribute(TextFlow *tb, int attr, void *val);

#endif
