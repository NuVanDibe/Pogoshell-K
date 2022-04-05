#ifndef TRICONTAINER_H
#define TRICONTAINER_H

#include "../window.h"

#define WIDGET_TRICONTAINER 0x1001
typedef struct
{
	Widget w;

	Widget *children[3];

} TriContainer;

TriContainer *tricontainer_new();
void tricontainer_setwidget(TriContainer *tri, int i, Widget *w);
void tricontainer_set_attribute(TriContainer *tc, int attr, void *val);


#endif
