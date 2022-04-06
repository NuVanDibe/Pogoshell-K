
#include <pogo.h>
#include "gba_defs.h"
#include "bitmap.h"
#include "window.h"
#include "text.h"
#include "misc.h"
#include "msgbox.h"

extern Screen *MainScreen;

int msgbox_yesno(tbox *box, char *text)
{
	TextBar *tb;
	int i, c, rc = -1;
	char p[strlen(text) + 24];

	//fprintf(stderr, "malloc = %p\n", p);
	sprintf(p, "%s\n(A) = %s  (B) = %s", text, TEXT(YES), TEXT(NO));
	textflow_set_attribute(box->txt, WATR_TEXT, p);

	tb = box->title;
	if (tb)
		textbar_set_attribute(tb, WATR_TEXT, TEXT(QUESTION));

	i = box->txt->w.width;
	if (tb && tb->w.width > i)
		i = tb->w.width;
	box->win->width = box->txt->w.width;

	i = box->txt->w.height;
	if (tb)
		i += tb->w.height;
	box->win->height = i;

	box->win->x = (240 - box->win->width) / 2;
	box->win->y = (160 - box->win->height) / 2;

	window_show(box->win);
	screen_redraw(MainScreen);

	while(rc == -1)
	{
		while((c = getchar()) == EOF)
			Halt();

		switch(c)
		{
			case RAWKEY_A:
				rc = 1;
				break;
			case RAWKEY_B:
				rc = 0;
				break;
		}
	}

	//textflow_set_attribute(box->txt, WATR_TEXT, "");

	window_hide(box->win);
	screen_redraw(MainScreen);
	return rc;

}

int msgbox_list(tbox *box, char *title, char **lines, int num)
{
	int i, c, h;
	int rc = -2;
	int marked = 0;
	TextBar *tb;

	if (!box->list)
		return -1;

	tricontainer_set_attribute((TriContainer *)box->win->widget, WATR_CHILD + 1, (void *)box->list);


	tb = box->title;	
	if (tb)
		textbar_set_attribute(tb, WATR_TEXT, title);

	listview_clear(box->list);
	for(i=0; i<num; i++)
		listview_addline(box->list, NULL, NULL, lines[i]);

	listview_set_marked(box->list, marked);

	listview_set_attribute(box->list, WATR_COLOR, &box->txt->textcolor);

	i = box->list->w.height;
	if (tb)
		i += tb->w.height;
	box->win->height = i;

	i = box->list->w.width;
	if(tb && tb->w.width > i) {
		i = tb->w.width;
		box->win->width = i;
	} else {
		box->win->width = i;
		if (box->list->backdrop)
			i -= box->list->backdrop->border*2;
	}
	listview_set_attribute(box->list, WATR_COLWIDTH, (void *) i);

	box->win->x = (240 - box->win->width) / 2;
	box->win->y = (160 - box->win->height) / 2;

	window_show(box->win);
	screen_redraw(MainScreen);

	while(rc == -2)
	{
		while((c = getchar()) == EOF)
			Halt();

		switch(c)
		{
			case RAWKEY_A:
				rc = marked;
				break;
			case RAWKEY_B:
				rc = -1;
				break;

			case RAWKEY_LEFT:
				h = listview_get_marked(box->list);
				listview_set_marked(box->list, marked - box->list->showing);
				if (h == listview_get_marked(box->list))
					listview_set_marked(box->list, 0);
				marked = listview_get_marked(box->list);
				screen_redraw(MainScreen);
				break;
			case RAWKEY_RIGHT:
				h = listview_get_marked(box->list);
				listview_set_marked(box->list, marked + box->list->showing);
				if (h == listview_get_marked(box->list))
					listview_set_marked(box->list, box->list->lines-1);
				marked = listview_get_marked(box->list);
				screen_redraw(MainScreen);
				break;
			case RAWKEY_UP:
				if(marked)
				{
					marked--;
					listview_set_marked(box->list, marked);
					screen_redraw(MainScreen);
				}
				break;

			case RAWKEY_DOWN:
				if(marked < (num-1))
				{
					marked++;
					listview_set_marked(box->list, marked);
					screen_redraw(MainScreen);
				}
				break;
		}
	}

	window_hide(box->win);
	tricontainer_set_attribute((TriContainer *)box->win->widget, WATR_CHILD + 1, (void *)box->txt);
	screen_redraw(MainScreen);

	return rc;
}

int msgbox_list2(tbox *box, char *title, char *str, int num)
{
	int rc, i;
	char *ptr, *start;
	char *lines[num];
	char tmp[strlen(str)];

	if (!box->list)
		return -1;

	strcpy(tmp, str);

	ptr = tmp;

	for(i=0; *ptr && (i<num); i++)
	{
		start = ptr;
		while(*ptr && *ptr != '|') ptr++;
		if(*ptr)
			*ptr++ = 0;
		lines[i] = start;
	}
	rc = msgbox_list(box, title, lines, num);

	return rc;
}

void msgbox_transient_show(tbox *box, char *title, char *text)
{
	TextBar *tb;
	int i;

	tb = box->title;
	if (tb)
		textbar_set_attribute(tb, WATR_TEXT, title);

	if (box->txt)
		textflow_set_attribute(box->txt, WATR_TEXT, text);

	i = 0;
	if (box->txt)
		i = box->txt->w.height;
	if (tb)
		i += tb->w.height;
	box->win->height = i;

	i = 0;
	if (box->txt)
		i = box->txt->w.width;
	if (tb && tb->w.width > i)
		i = tb->w.width;
	box->win->width = i;

	box->win->x = (240 - box->win->width) / 2;
	box->win->y = (160 - box->win->height) / 2;


	window_show(box->win);
	screen_redraw(MainScreen);
}

void msgbox_transient_hide(tbox *box)
{
	window_hide(box->win);
	screen_redraw(MainScreen);
}

void msgbox_info(tbox *box, char *title, char *text)
{
	int c = 0;

	msgbox_transient_show(box, title, text);

	while(c < RAWKEY_START || c > 0x80)
	{
		while((c = getchar()) == EOF)
			Halt();
	}

	msgbox_transient_hide(box);
}
