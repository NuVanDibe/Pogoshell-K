#ifndef TEXTREADER_H
#define TEXTTEADER_H

#include "../window.h"
#include "../backdrop.h"
#include "typeface.h"

#define WIDGET_TEXTREADER 0x1010
typedef struct
{
	Widget w;
	BackDrop *backdrop;


} TextReader;


enum {FONT_TEXT, FONT_BOLD, FONT_EMPH, FONT_BIG};


TextReader *textreader_new();
void textreader_set_attribute(TextReader *tr, int attr, void *val);

#endif
