
#include <pogo.h>

#include "window.h"
#include "widgets/listview.h"
#include "text.h"
#include "misc.h"

#include "settings.h"

extern ListView *MainList;
extern Screen *MainScreen;

enum { ST_YESNO, ST_SORT, ST_SLEEP, ST_THEME };

const static uchar stypes[NO_SETTINGS] = { ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_YESNO, ST_SORT, ST_YESNO, ST_SLEEP, ST_YESNO, ST_THEME };
const static char *sort_types[] = { "Name", "Type", "Size", "None" };
const static char *sleep_types[] = { "Never", "1 Min", "3 Min", "5 Min", "10 Min", "30 Min" };
unsigned /*short*/char settings[NO_SETTINGS];
const static unsigned /*short*/char defsettings[NO_SETTINGS] = {0, 1, 1, 1, 0, 0, 0, 1, 2, 1, 0};

char temp_theme_name[32];
char theme_count = 1;

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
	char *tmp;

	if(stypes[i] == ST_YESNO)
		p = (settings[i] ? TEXT(YES) : TEXT(NO));
	else if(stypes[i] == ST_SORT)
		p = sort_types[settings[i]];
	else if(stypes[i] == ST_SLEEP) {
		p = sleep_types[settings[i]];
	} else {
		get_theme_name(settings[i], temp_theme_name);
		tmp = strchr(temp_theme_name, '.');
		if (tmp)
			*tmp = '\0';
		//get_theme_name(0, temp_theme_name);
		p = temp_theme_name;
	}

	listview_setline(MainList, i, icon, TEXT(SETTINGS_START + i), p);
}

void settings_icon(BitMap *bm)
{
	icon = bm;
}

void settings_init(void)
{
	int i;
	DIR *dir;
	struct dirent *de;
	struct stat sbuf;
	char *tmp;

	memcpy(settings, defsettings, NO_SETTINGS);

	dir = opendir(GET_PATH(THEMES));
	if (dir)
	{
		for (i = 0; ((de = readdir(dir)) && (i < MAX_FILE_COUNT)); i++)
		{
			tmp = strrchr(de->d_name, '.');
			stat(de->d_name, &sbuf);
			if (!(sbuf.st_mode & S_IFDIR) && strcmp(tmp, ".theme") == 0) {
				if (strcmp(de->d_name, "default.theme") != 0)
					theme_count++;
			}
		}
		closedir(dir);
	}
}

void get_theme_name(char line, char *dest)
{
	int i;
	char count, current;
	DIR *dir;
	struct dirent *de;
	struct stat sbuf;
	char *tmp;

	dir = opendir(GET_PATH(THEMES));
	count = 1;
	if (dir)
	{
		for (i = 0; (de = readdir(dir)) && (i < MAX_FILE_COUNT); i++)
		{
			tmp = strrchr(de->d_name, '.');
			stat(de->d_name, &sbuf);
			if (!(sbuf.st_mode & S_IFDIR) && strcmp(tmp, ".theme") == 0) {
				if (strcmp(de->d_name, "default.theme") == 0)
					current = 0;
				else
					current = count++;
				if (line == current) {
					memcpy(dest, de->d_name, 32);
					closedir(dir);
					return;
				}
			}
		}
		closedir(dir);
	}
}

/*int*/void settings_edit(void)
{
	int i,c,h,m;
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

		case RAWKEY_LEFT:
			h = listview_get_marked(MainList);
			listview_set_marked(MainList, marked - MainList->showing);
			if (h == listview_get_marked(MainList))
				listview_set_marked(MainList, 0);
			break;
		case RAWKEY_RIGHT:
			h = listview_get_marked(MainList);
			listview_set_marked(MainList, marked + MainList->showing);
			if (h == listview_get_marked(MainList))
				listview_set_marked(MainList, MainList->lines-1);
			break;
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
			switch (stypes[marked]) {
				case ST_SORT:
					m = 4;
					break;
				case ST_THEME:
					m = theme_count;
					break;
				case ST_SLEEP:
					m = 6;
					break;
				case ST_YESNO:
				default:
					m = 2;
					break;
			}
			settings[marked] %= m;
			update_line(marked);
			if (marked == 7)
				 listview_set_dirty(MainList);
			break;
		case RAWKEY_B:
			return /* 0 */;
		}
		
		screen_redraw(MainScreen);
	}
}
