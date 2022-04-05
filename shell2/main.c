
#include <pogo.h>
#include <rtc.h>

#include "gba_defs.h"
#include "bitmap.h"
#include "window.h"
#include "filetype.h"
#include "filesys.h"
#include "savesystem.h"
#include "freespace.h"
#include "text.h"
#include "users.h"
#include "settings.h"
#include "misc.h"
#include "msgbox.h"
#include "widgets/widgets.h"
#include "guiparser.h"

/*extern uint32 *mem_base;

extern int theme_count;
char theme_name[32];
*/
int sram_fd = -1;
#define free_space() ioctl(sram_fd, SR_FREESPACE)

extern void SoftReset(unsigned char flags);
// Device init funtions
void filesys_init();
void deb_init();
void screen_init();
void key_init();
void sram_init();
void gamesys_init();

int sram_convert(void);
void bmp_view(char *fname);
void jpg_view(char *fname);

TextBar *StatusBar;
TextBar *TitleBar;
ListView *MainList;
BitMap *IconSet;
int IconHeight;

Screen *MainScreen;
Window *MessageWin;
TextFlow *MessageTxt;
ListView *MessageList;
BackDrop *ListBar;

FILE *config_fp;

const char *pathname[4] = {"PLUGINS", "THEMES", "FONTS", "BITMAPS"};
char *path[4];

int sram_game_size = 64;

uint16 marked;

const char *PogoVersion = "2.0b3mod5";

/* State, saved to  /sram/.state */
struct {
	unsigned /*short*/ char settings[NO_SETTINGS];
	uint16 marked;
}  __attribute__ ((packed)) state;

/* Save current state to SRAM */
void save_state(void)
{
	int fd;
	uchar usr;
	char *name = filesys_get_current();

	memcpy(state.settings, settings, NO_SETTINGS);
	state.marked = marked;
	
	fd = open("/sram/.state", O_CREAT);
	if(fd >= 0)
	{
		write(fd, &state, sizeof(state));
		write(fd, name, strlen(name)+1);
		close(fd);
	}

	sram_setuser(0);

	fd = open("/sram/.glbstate", O_CREAT);
	if(fd >= 0)
	{
		usr = 0;
		write(fd, &usr, 1);
		usr = CurrentUser;
		write(fd, &usr, 1);
		close(fd);
	}

	sram_setuser(CurrentUser);

}

enum {LOCAL, ALL};

/* Load current state from SRAM */
int load_state(int what)
{
	char tmp[100];
	int fd;
	uchar usr;


	if(what == ALL)
	{
		sram_setuser(0);

		fd = open("/sram/.glbstate", 0);
		if(fd >= 0)
		{
			read(fd, &usr, 1);
			read(fd, &usr, 1);
			CurrentUser = usr;
			update_user();
			close(fd);
		}

		sram_setuser(CurrentUser);
	}

	fd = open("/sram/.state", 0);
	if(fd >= 0)
	{
		read(fd, &state, sizeof(state));
		read(fd, tmp, 100);
		close(fd);
		filesys_cd_marked(tmp);
		marked = state.marked;
		memcpy(settings, state.settings, NO_SETTINGS);
		/*if (settings[SF_THEME] >= theme_count)
			settings[SF_THEME] = 0;
		get_theme_name(settings[SF_THEME], theme_name);*/
		return 1;
	}

	return 0;
}

char *clipboard = (char *)(FREEPTR);
char clipname[36];
int clipsize;

int sram_copy(char *name)
{
	int l;
	int fd;
	struct stat sb;

	stat(name, &sb);
	if(sb.st_mode & S_IFDIR)
		return -4;
	
	fd = open(name, 0);
	if(fd >= 0)
	{
		//l = lseek(fd, 0, SEEK_END);
		//lseek(fd, 0, SEEK_SET);
		l = sb.st_size;

		if(l > 128*1024)
			return -2;

		if(l >= 0)
		{

			strcpy(clipname, basename(name));

			read(fd, clipboard, l);
			clipsize = l;
			close(fd);
			return 0;
		}
		else
			return -3;
	}
	return -1;
}

int sram_cut(char *name)
{
	if(sram_copy(name) == 0)
	{
		return remove(name);
	}
	return -1;
}

int sram_del(char *name)
{
	return remove(name);
}

int sram_paste(char *name)
{
	char dest[42];
	int fd;

	if(!clipsize)
		return -3;

	if(!name)
	{
		name = basename(clipname);
	}

	sprintf(dest, "/sram/%s", name);

	if(free_space() > clipsize+8)
	{
		fd = open(dest, 0);
		if(fd >= 0)
		{
			close(fd);
			return -1;
		}
		fd = open(dest, O_CREAT);
		if(fd >= 0)
		{
			write(fd, clipboard, clipsize);
			close(fd);
			return 0;
		}
		return -4;
	}
	return -2;
}

struct tm clockdata;
char *oldtext = NULL;
void statusbar_set(char *text)
{
	char tmp[80];
	if(StatusBar)
	{
		oldtext = text;

		if(rtc_check())
		{
			//memcpy(&dst, localtime(time(NULL)), sizeof(struct tm));
			strcpy(tmp, text);
			sprintf(&tmp[strlen(tmp)], "| %02d:%02d", clockdata.tm_hour, clockdata.tm_min);
			text = tmp;
		}
		textbar_set_attribute(StatusBar, WATR_TEXT, text);
	}
}


void statusbar_refresh(void)
{
	if(oldtext)
		statusbar_set(oldtext);
}

void pprintf(char *tmp, char *fmt)
{
	char *s;
	while(*fmt)
	{
		if(*fmt == '%')
		{
			switch(fmt[1])
			{
			case 'v':
				strcpy(tmp, PogoVersion);
				tmp += strlen(PogoVersion);
				break;
			case 'p':
				s = filesys_get_current();
				strcpy(tmp, s);
				tmp += strlen(tmp);
				break;
			case 'u':
				strcpy(tmp, UserName);
				tmp += strlen(tmp);
				break;
			case 'f':
				itoa(free_space(), tmp, 10);
				tmp += strlen(tmp);
				break;
			case 'F':
				itoa(free_space() / 1024, tmp, 10);
				tmp += strlen(tmp);
				break;
			case 'm':
				itoa(memory_avail() * 4, tmp, 10);
				tmp += strlen(tmp);
				break;
			case 'M':
				itoa(memory_avail() / 256, tmp, 10);
				tmp += strlen(tmp);
				//itoa(((memory_avail() & ~1024) * 10) / 1024, tmp, 10);
				break;
				
			}
			fmt++;
		}
		else
			*tmp++ = *fmt;

		fmt++;
	}
	*tmp = 0;
}



void init_devices(void)
{
	filesys_init();
	deb_init();
	screen_init();
	key_init();

	gamesys_init();
}

// Takes a lot of bloody space but I have no better solution currently
//DirList dirlist[MAX_FILE_COUNT];
//char dirsize[MAX_FILE_COUNT][10];
//char dirname[MAX_FILE_COUNT][32];
DirList *dirlist = (DirList *) 0x02000000;
char *dirsize = (char *) (0x02000000+sizeof(DirList)*MAX_FILE_COUNT);
char *dirname = (char *) (0x02000000+(sizeof(DirList)+10)*MAX_FILE_COUNT);
//#if MODPTR != (0x02000000+(sizeof(DirList)+10+32)*MAX_FILE_COUNT)
//#error("You must update MODPTR")
//#endif

BitMap BackgroundBM;

void update_list(void)
{
	char tmp[80];
	int i, t, filecount;

	listview_clear(MainList);
	filecount = filesys_getfiles(dirlist);
	for(i=0; i<filecount; i++)
	{
		if(dirlist[i].type || settings_get(SF_HIDESIZE) || (filesys_getstate() == FSTATE_GAMES))
			dirsize[i*10] = 0;
		else
			sprintf(&dirsize[i*10], "%d", dirlist[i].size);

		t = filetype_lookup(&dirlist[i]);
/*
		//fprintf(stderr, "%s\n", dirlist[i].name);
		if(strcmp(dirlist[i].name, ".background.bm") == 0)
		{
			FILE *fp = fopen(filesys_fullname(i), "rb");
			BitMap *bmap = bitmap_readbm(fp);
			fprintf(stderr, "BG %s %p %p\n", dirlist[i].name, fp, bmap);
			memcpy(&BackgroundBM, bmap, sizeof(BitMap));
			free(bmap);
			fclose(fp);
			backdrop_set_attribute(MainList->backdrop, WATR_BITMAP, &BackgroundBM);
		}
*/		
		strcpy(&dirname[i*32], dirlist[i].name);
		if((t > 1) && settings_get(SF_HIDEEXT))
		{
			char *p = strrchr(&dirname[i*32], '.');
			if(p && p != &dirname[i*32])
				*p = 0;
		}

		listview_addline(MainList, filetype_icon(t), &dirname[i*32], &dirsize[i*10]);
	}

	listview_set_marked(MainList, marked);

	if(filesys_getstate() == FSTATE_SRAM)
		pprintf(tmp, TEXT(TITLEBAR_SRAM));
	else
		pprintf(tmp, TEXT(TITLEBAR_MAIN));

	textbar_set_attribute(TitleBar, WATR_TEXT, tmp);
	//statusbar_refresh();
}


static BitMap **icon_list;

void textreader_set_font(int n, Font *f);


void setup_screen(void)
{
	char tmp[80];
	int i, count;
	char *theme;
	Widget *root;
	Font *font;
	
	FILE *fp;
	Widget *mbox;
	Window *win; 
	//BitMap *screen = bitmap_getscreen();
	//bitmap_clear(screen, 0xFF00);

	MainScreen = screen_new();
	win = window_new(MainScreen, 0, 0, 240, 160);

	guiparser_readsymbols(config_fp);

	theme = (char *)0x02000000;

	strcpy(tmp, GET_PATH(THEMES));
	//strcat(tmp, theme_name);
	strcat(tmp, "default.theme");
	//fp = fopen(".shell/themes/default.theme", "rb");
	fp = fopen(tmp, "rb");
	i = fread(theme, 4096, 1, fp);
	theme[i] = 0;
	fclose(fp);

	root = guiparser_create(theme, "root");

	mbox = guiparser_findwidget("msgbox");

	StatusBar = (TextBar *)guiparser_findwidget("status");
	TitleBar = (TextBar *)guiparser_findwidget("title");
	MainList = (ListView *)guiparser_findwidget("list");
	MessageTxt = (TextFlow *)guiparser_findwidget("mflow");

	ListBar = MainList->scrollbar;

	if(mbox)
	{
		//textflow_set_attribute(MessageTxt, WATR_TEXT, "This is just a little\ntest of the messagebox,\nI hope it works well!\n--Sasq");

		MessageWin = window_new(MainScreen, 50, 50, 140, 60);
		//fprintf(stderr, "msgwin %p mbox %p\n", MessageWin, mbox);
		window_setwidget(MessageWin, mbox);
		window_hide(MessageWin);
		MessageList = listview_new(1, 10, MessageTxt->font);
		listview_set_attribute(MessageList, WATR_MARGIN, (void *)2);
		listview_set_attribute(MessageList, WATR_MARGIN+1, (void *)2);

		if(MessageTxt->backdrop)
			listview_set_attribute(MessageList, WATR_BACKDROP, MessageTxt->backdrop);
	}

	count = IconSet->height / IconHeight;

	icon_list = malloc(count *4);

	for(i=0; i<count; i++)
	{
		icon_list[i] = bitmap_new(IconSet->width, IconHeight, BPP16 | TRANSPARENT | DONT_ALLOC);
		icon_list[i]->pixels = (uint16 *)IconSet->pixels + IconSet->width * i * IconHeight;
	}

	filetype_set_iconset(icon_list);
	filetype_readtypes(config_fp);

	font = font_load_path("dungeon.font");
	textreader_set_font(FONT_TEXT, font);

	font = font_dup(font);
	font->flags |= FFLG_BOLD;
	textreader_set_font(FONT_BOLD, font);

	font = font_load_path("dungeoni.font");
	textreader_set_font(FONT_EMPH, font);

	font = font_load_path("dungeon12.font");
	textreader_set_font(FONT_BIG, font);

	window_setwidget(win, root);

	update_list();
}

void cmd_about(char *dummy)
{
	msgbox_info("About PogoShell 2.0 Beta", "PogoShell v2.0 (Beta 3 mod5 release)\nCreated by Sasq in 2003/2004\nAltered by Kuwanger in 2006\nPress SELECT for help\nat anytime. Enjoy!");
}

// Commands

void cmd_sramcopy(char *name)
{
	char tmp[50];
	int i;
	
	if(!name || !strlen(name))
		return;
	
	i = sram_copy(name);
	sprintf(tmp, TEXT(SRAM_COPY), basename(name), i);
	statusbar_set(tmp);
}

void cmd_srampaste(char *name)
{
	char tmp[50];
	int i = sram_paste(NULL);
	sprintf(tmp, TEXT(SRAM_PASTE), basename(clipname), i);
	statusbar_set(tmp);
	if(i >= 0)
		update_list();
}

void cmd_sramdel(char *name)
{
	char tmp[50];
	int i;
	
	//fprintf(stderr, "Delete %s\n", name);
	sprintf(tmp, TEXT(WISH_SRAM_DEL), basename(name));
	if(msgbox_yesno(tmp) == 1)
	{
		i = sram_del(name);
		sprintf(tmp, TEXT(SRAM_DEL), basename(name), i);
		statusbar_set(tmp);
		if(i >= 0)
			update_list();
	}
}

void cmd_switchuser(char *name)
{
	char tmp[50];

	save_state();
	switch_user();
	load_state(LOCAL);
	sprintf(tmp, TEXT(SWITCHED_USER), UserName);
	statusbar_set(tmp);
	update_list();
}

void cmd_help(char *name)
{
	int i = filesys_getstate();
	switch(i)
	{
	case FSTATE_NORMAL:
		msgbox_info(TEXT(MAIN_HELP_TITLE), TEXT(MAIN_HELP_TEXT));
		break;
	case FSTATE_SRAM:
	default:
		msgbox_info(TEXT(SRAM_HELP_TITLE), TEXT(SRAM_HELP_TEXT));
		break;
	}
}

void cmd_settings(char *name)
{
	/*int r;

	r =*/ settings_edit();
	if(settings_get(SF_SCROLLBAR))
		MainList->scrollbar = ListBar;
	else
		MainList->scrollbar = NULL;
	save_state();
	/*if (r) {
		mem_base = NULL;
		((void(*)(void))0x08000000)();
	}*/
	update_list();
}

typedef void (*CmdFunc)(char *);

CmdFunc commands[] = { cmd_settings, cmd_about, cmd_switchuser, cmd_help, cmd_sramcopy, cmd_srampaste, cmd_sramdel };


void Halt(void);

static int qualifiers = 0;
int show_text(char *cmd, char *fname, int keys);
int vkey_get_qualifiers(void);

extern char *sramfile_mem;
extern int sram_size;
int sram_strcmp(const char *a, const char *b);

int main(int argc, char **argv)
{
	uchar tmp[50];
	FILE *fp;
	int c, i;
	int converted = 0;
	int have_state;
	int count = 0;
	int srsize = -1;
	//uint32 *mem = (uint32 *)0x02000000;

	SoftReset(0xfc);

	rtc_enable();
	i = rtc_check();
	rtc_get(tmp);

	/* I assume this is a check if we're in an emulator, to avoid
	 * display the splash screen.  This is overwritten if there's
	 * a settings section in pogo.cfg, however, and I don't really
	 * see the point of turning off the splash screen on an emulator.
	 *
	c = 0;
	for(i=0; i<256; i++)
		c += mem[i*256];
         */

	init_devices();

	config_fp = fopen(".shell/pogo.cfg", "rb");
	if(!config_fp)
		config_fp = fopen(".pogo.cfg", "rb");
	//fprintf(stderr, "open %p\n", config_fp);

	if(find_section(config_fp, "settings"))
	{
		char *name, *val;
		while((c = read_line(tmp, sizeof(tmp), config_fp)) >= 0)
		{
			if(c > 0)
			{
				if(parse_assignment(tmp, &name, &val))
				{
					if(strcmp(name, "SRAM") == 0)
					{
						srsize = atoi(val);
					}
					else
					/*if(strcmp(name, "GAME") == 0)
					{
						sram_game_size = atoi(val);
					}
					else*/
					for(i=0; i<4; i++)
						if(strcmp(name, pathname[i]) == 0)
						{
							char *p;
							int l = strlen(val);
							path[i] = malloc(l+2);
							strcpy(path[i], val);
							p = &path[i][l-1];
							if(*p != '/')
							{
								p[1] = '/';
								p[2] = 0;
							}
							//path[i] = strdup(val);
						}
				}
			}
		}

	}

	if(srsize)
	{
		sram_size = srsize * 1024;
		sramfile_mem = (uchar *)0x0E040000 - sram_size;
	}

	converted = sram_convert();

	sram_init();

	sram_fd = open("/sram", 0);

	filesys_cd_marked("");

	read_texts(config_fp);
	read_users(config_fp);

	settings_init();
	//strcpy(theme_name, "default.theme");

	have_state = load_state(ALL);

	//dprint("blah\n");
	//time(NULL);

	//settings_read();

	fp = fopen(".shell/splash.bmp", "rb");
	fclose(fp);

	if(fp)
	{
		bmp_view(".shell/splash.bmp");
	}

	setup_screen();

	fclose(config_fp);

	if(settings_get(SF_SCROLLBAR))
		MainList->scrollbar = ListBar;
	else
		MainList->scrollbar = NULL;


	if((fp = fopen("/sram/.bookmark", "rb")))
	{
		unsigned int l;
		fread(&l, 1, 4, fp);
		if(l & 0x80000000)
		{
			l &= 0x7FFFFFFF;
			fread(tmp, 1, sizeof(tmp), fp);
			fclose(fp);
			show_text("", tmp, 0);
		}
	}

	time2(&clockdata);
	if(converted)
		statusbar_set("SRAM converted!");
	else
		statusbar_set(TEXT(INTRO_LINE));

	screen_redraw(MainScreen);

	if(!have_state)
	{
		cmd_about(NULL);
		save_state();
	}

	savesys_savelastgame();

	while(1)
	{
		int h;
		marked = listview_get_marked(MainList);
		count = 0;
		while((c = getchar()) == EOF)
		{
			count++;
			Halt();

			if(count == 50)
			{
				if(rtc_check())
				{
					h = clockdata.tm_min;
					time2(&clockdata);
					if(h != clockdata.tm_min)
					{
						statusbar_refresh();
						screen_redraw(MainScreen);
					}
				}
				count = 0;
			}
		}


		qualifiers = vkey_get_qualifiers();

		switch(c)
		{

		case RAWKEY_LEFT:
			filesys_update_key(0x2);
			h = listview_get_marked(MainList);
			listview_set_marked(MainList, marked - MainList->showing);
			if (h == listview_get_marked(MainList))
				listview_set_marked(MainList, 0);
			break;
		case RAWKEY_RIGHT:
			filesys_update_key(0x3);
			h = listview_get_marked(MainList);
			listview_set_marked(MainList, marked + MainList->showing);
			if (h == listview_get_marked(MainList))
				listview_set_marked(MainList, MainList->lines-1);
			break;
		case RAWKEY_UP:
			filesys_update_key(0x0);
			if(qualifiers == 1)
				listview_set_marked(MainList, 0);
			else
			if(qualifiers == 2)
				listview_set_marked(MainList, marked - MainList->showing);
			else
				listview_set_marked(MainList, marked-1);
			break;

		case RAWKEY_DOWN:
			filesys_update_key(0x1);
			if(qualifiers == 1)
				listview_set_marked(MainList, MainList->lines-1);
			else
			if(qualifiers == 2)
				listview_set_marked(MainList, marked + MainList->showing);
			else
				listview_set_marked(MainList, marked+1);
			break;

		case RAWKEY_A:
			if(filesys_getstate() == FSTATE_SRAM)
			{
				char *name = filesys_fullname(marked);
				switch(qualifiers)
				{
				case 0:
					i = filetype_lookup(&dirlist[marked]);
					if(filetype_handle(filesys_fullname(marked), i, 0) == 2)
						update_list();
					break;
				case 2:
					cmd_sramcopy(name);
					break;
				case 1:
					cmd_srampaste(name);
					break;
				case 3:
					cmd_sramdel(name);
					break;
				}
			}
			else
			{
				i = filetype_lookup(&dirlist[marked]);
				if (filetype_handle(filesys_fullname(marked), i, qualifiers) == 2)
					update_list();
			}
			break;
			
		case RAWKEY_B:
			if(qualifiers == 2)
				filesys_cd_marked("");
			else
			if(qualifiers == 1)
				filesys_cd_marked("/sram");
			else
				filesys_parent();
			update_list();
			break;

		case RAWKEY_START:
			if(qualifiers ==  1)
			{
				cmd_settings(NULL);
			}
			else
			{
				char *name = filesys_fullname(marked);
				i = 5;
				strcpy(tmp, TEXT(CMD_BASIC));
				if(filesys_getstate() == FSTATE_SRAM)
				{
					strcat(tmp, TEXT(CMD_SRAM));
					i += 2;
				}
				i = msgbox_list2(TEXT(COMMANDS), tmp, i);

				if(i >= 0)
					commands[i](name);
			}
			break;

		case RAWKEY_SELECT:
			if(qualifiers ==  1)
				cmd_switchuser(NULL);
			else
			if(qualifiers == 3)
				cmd_about(NULL);
			else
			if(qualifiers == 0)
				cmd_help(NULL);
			break;
		}

		if (filesys_change_hidden_dir())
			update_list();

		screen_redraw(MainScreen);

	}

	return 0;
}

