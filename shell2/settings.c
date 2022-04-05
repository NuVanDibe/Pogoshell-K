
#include <pogo.h>

#include "window.h"
#include "widgets/listview.h"
#include "text.h"

#include "settings.h"

extern ListView *MainList;
extern Screen *MainScreen;

enum { ST_YESNO, ST_SORT  };

const static uchar stypes[NO_SETTINGS] = { ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_SORT, ST_YESNO };
const static char *sort_types[] = { "Name", "Type", "Size" };
uchar settings[NO_SETTINGS];
const static uchar defsettings[NO_SETTINGS] = {0, 1, 1, 1, 0, 0, 0, 1};

int vkey_get_qualifiers(void);

/*
int settings_read(void)
{
	FILE *fp;
	if((fp = fopen("/sram/.settings", "rb")))
	{
		fread(settings, 1, NO_SETTINGS, fp);
		fclose(fp);
		return 1;
	}
	memset(settings, 0, NO_SETTINGS);
	return 0;
}

int settings_write(void)
{
	FILE *fp;
	if((fp = fopen("/sram/.settings", "wb")))
	{
		fwrite(settings, 1, NO_SETTINGS, fp);
		fclose(fp);
		return 1;
	}
	return 0;
}
*/
static BitMap *icon;

void update_line(int i)
{
	const char *p;
	if(stypes[i] == ST_YESNO)
		p = (settings[i] ? TEXT(YES) : TEXT(NO));
	else
	//if(stypes[i] == ST_SORT)
		p = sort_types[settings[i]];

	listview_setline(MainList, i, icon, TEXT(SETTINGS_START + i), p);
}

void settings_icon(BitMap *bm)
{
	icon = bm;
}


void settings_init(void)
{
	memcpy(settings, defsettings, NO_SETTINGS);
}

void settings_edit(void)
{
	int i,c;
	int marked = 0;
	int qualifiers = 0;

	listview_clear(MainList);

	for(i=0; i<NO_SETTINGS; i++)
	{
		listview_addline(MainList, icon, "", "");
		update_line(i);
	}

	listview_set_marked(MainList, marked);
	screen_redraw(MainScreen);

	while(1)
	{
		marked = listview_get_marked(MainList);

		while((c = getchar()) == EOF)
			Halt();

		qualifiers = vkey_get_qualifiers();

		switch(c)
		{

		case RAWKEY_UP:
			if(qualifiers == 1)
				listview_set_marked(MainList, 0);
			else
			if(qualifiers == 2)
				listview_set_marked(MainList, marked - MainList->showing);
			else
				listview_set_marked(MainList, marked-1);
			break;

		case RAWKEY_DOWN:
			if(qualifiers == 1)
				listview_set_marked(MainList, MainList->lines-1);
			else
			if(qualifiers == 2)
				listview_set_marked(MainList, marked + MainList->showing);
			else
				listview_set_marked(MainList, marked+1);
			break;

		case RAWKEY_A:
			settings[marked]++;
			if(stypes[marked] == ST_YESNO) settings[marked] = (settings[marked] % 2);
			if(stypes[marked] == ST_SORT) settings[marked] = (settings[marked] % 3);
			update_line(marked);
			break;
		case RAWKEY_B:
			return;
		}

		screen_redraw(MainScreen);


	}
}
