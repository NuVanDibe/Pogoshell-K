#ifndef TYPEFACE_H
#define TYPEFACE_H

#include "../window.h"
#include "../backdrop.h"

typedef struct
{
	Font *font;
	Color shadow;
	Color outline;
	int style;
	uchar global;
} Typeface;

Typeface *typeface_new(Font *font, uchar global);
void typeface_set_attribute(Typeface *tf, int attr, void *val);

#endif
