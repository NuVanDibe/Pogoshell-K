#ifndef MSGBOX_H
#define MSGBOX_H

#include "widgets/widgets.h"

typedef struct
{
	Window *win;
	TextBar *title;
	TextFlow *txt;
	ListView *list;

} tbox;

int msgbox_yesno(tbox *box, char *text);
int msgbox_list(tbox *box, char *title, char **lines, int num);
void msgbox_transient_show(tbox *box, char *title, char *text);
void msgbox_transient_hide(tbox *box);
void msgbox_info(tbox *box, char *title, char *text);
int msgbox_list2(tbox *box, char *title, char *str, int num);

#endif
