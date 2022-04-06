#include <pogo.h>
#include "gba_defs.h"
#include "bitmap.h"
#include "window.h"
#include "text.h"
#include "widgets/widgets.h"
#include "guiparser.h"
#include "misc.h"

extern Window *MessageWin;
extern TextFlow *MessageTxt;
extern Screen *MainScreen;
extern ListView *MessageList;

#define MSGBOX_TITLE "mtitle"
#define MSGBOX_TEXT "mflow"


int msgbox_yesno(char *text)
{
	Widget *w;
	int c, rc = -1;
	char *p = malloc(strlen(text) + 24);
	//fprintf(stderr, "malloc = %p\n", p);
	sprintf(p, "%s\n(A) = %s  (B) = %s", text, TEXT(YES), TEXT(NO));
	textflow_set_attribute(MessageTxt, WATR_TEXT, p);

	if((w = guiparser_findwidget(MSGBOX_TITLE)))
		textbar_set_attribute((TextBar *)w, WATR_TEXT, TEXT(QUESTION));

	MessageWin->width = MessageTxt->w.width + 8;
	MessageWin->height = MessageTxt->w.height + 8 + 12;

	MessageWin->x = (240 - MessageWin->width) / 2;
	MessageWin->y = (160 - MessageWin->height) / 2;

	window_show(MessageWin);
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

	//textflow_set_attribute(MessageTxt, WATR_TEXT, "");
	free(p);

	window_hide(MessageWin);
	screen_redraw(MainScreen);
	return rc;

}

int msgbox_list(char *title, char **lines, int num)
{
	int i, c;
	int rc = -2;
	int marked = 0;
	Widget *w;
	tricontainer_set_attribute((TriContainer *)MessageWin->widget, WATR_CHILD + 1, (void *)MessageList);

	
	if((w = guiparser_findwidget(MSGBOX_TITLE)))
		textbar_set_attribute((TextBar *)w, WATR_TEXT, title);

	listview_clear(MessageList);
	for(i=0; i<num; i++)
		listview_addline(MessageList, NULL, lines[i]);

	listview_set_marked(MessageList, marked);

	listview_set_attribute(MessageList, WATR_COLOR, &MessageTxt->textcolor);

	//MessageWin->height = MessageList->w.height + 8;
	i = MessageList->w.height;
	if (w)
		i += w->height;
	// i += 4;
	MessageWin->height = i;
	i = MessageList->w.width;
	if(w && w->width > i) {
		i = w->width;
		MessageWin->width = i;
	} else {
		MessageWin->width = i;
		if (MessageList->backdrop)
			i -= MessageList->backdrop->border*2;
	}
	//i += 4;
	listview_set_attribute(MessageList, WATR_COLWIDTH, i);
	MessageWin->x = (240 - MessageWin->width) / 2;
	MessageWin->y = (160 - MessageWin->height) / 2;

	window_show(MessageWin);
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

			case RAWKEY_UP:
				if(marked)
				{
					marked--;
					listview_set_marked(MessageList, marked);
					screen_redraw(MainScreen);
				}
				break;

			case RAWKEY_DOWN:
				if(marked < (num-1))
				{
					marked++;
					listview_set_marked(MessageList, marked);
					screen_redraw(MainScreen);
				}
				break;
		}
	}

	window_hide(MessageWin);
	tricontainer_set_attribute((TriContainer *)MessageWin->widget, WATR_CHILD + 1, (void *)MessageTxt);
	screen_redraw(MainScreen);

	return rc;
}

int msgbox_list2(char *title, char *str, int num)
{
	int rc, i;
	char *ptr, *start;
	char **lines = malloc(num *4);
	char *tmp = strdup(str);

	ptr = tmp;

	for(i=0; *ptr && (i<num); i++)
	{
		start = ptr;
		while(*ptr && *ptr != '|') ptr++;
		if(*ptr)
			*ptr++ = 0;
		lines[i] = start;
	}
	rc = msgbox_list(title, lines, num);

	free(tmp);
	free(lines);
	return rc;
}

void msgbox_info(char *title, char *text)
{
	Widget *w;
	int c = 0;

	if((w = guiparser_findwidget(MSGBOX_TITLE)))
		textbar_set_attribute((TextBar *)w, WATR_TEXT, title);

	if((w = guiparser_findwidget(MSGBOX_TEXT)))
		textflow_set_attribute((TextFlow *)w, WATR_TEXT, text);

	MessageWin->width = MessageTxt->w.width + 8;
	MessageWin->height = MessageTxt->w.height + 8 + 12;

	MessageWin->x = (240 - MessageWin->width) / 2;
	MessageWin->y = (160 - MessageWin->height) / 2;

	window_show(MessageWin);
	screen_redraw(MainScreen);

	while(c < RAWKEY_START || c > 0x80)
	{
		while((c = getchar()) == EOF)
			Halt();
	}

	window_hide(MessageWin);
	screen_redraw(MainScreen);

}
