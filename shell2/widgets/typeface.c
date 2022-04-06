
#include "../settings.h"
#include "../bitmap.h"
#include "../window.h"
#include "typeface.h"

Typeface *typeface_new(Font *font, uchar global)
{
	Typeface *tf = malloc(sizeof(Typeface));

	tf->font = font;
	tf->style = 0;
	tf->shadow = Black_Color;
	tf->outline = White_Color;
	tf->global = global;

	return tf;
}

void typeface_set_attribute(Typeface *tf, int attr, void *val)
{
	Color c;
	int n = attr & 0xf;
	uint32 l;

	switch(attr & 0xFFF0)
	{
	case WATR_FONT:
		if(tf->font)
			free(tf->font);
		tf->font = (Font *)val;
		tf->font->flags |= tf->style;
		break;
	case WATR_RGB:
		l = (int) val;
		c.r = (l>>16) & 0xff;
		c.g = (l>>8) & 0xff;
		c.b = l & 0xff;
		c.a = l>>24;

		if (n & 1)
			tf->outline = c;
		else
			tf->shadow = c;
		break;
	case WATR_COLOR:
		if (n & 1)
			tf->outline = *((Color *)val);
		else
			tf->shadow = *((Color *)val);
		break;
	case WATR_STYLE:
		tf->style = (int) val;
		if (tf->font)
			tf->font->flags |= tf->style;
		break;
	}
}

