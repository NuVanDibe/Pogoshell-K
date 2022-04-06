#ifndef TEXTREADER_H
#define TEXTTEADER_H

#include "../window.h"
#include "../backdrop.h"

#define WIDGET_TEXTREADERPLUS 0x1011
typedef struct
{
	Widget w;
	BackDrop *backdrop;


} TextReaderPlus;


enum {FONT_TEXT, FONT_BOLD, FONT_EMPH, FONT_BIG};


TextReader *textreaderplus_new();
void textreaderplus_set_attribute(TextReader *tr, int attr, void *val);

#endif
