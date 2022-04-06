
#include <pogo.h>
#include <rtc.h>

#include "gba_defs.h"
#include "bitmap.h"
#include "window.h"
#include "filetype.h"
#include "filesys.h"
#include "savesystem.h"
#include "jpgview.h"
#include "text.h"
#include "users.h"
#include "settings.h"
#include "misc.h"
#include "widgets/widgets.h"
#include "msgbox.h"
#include "guiparser.h"

//extern uint32 *mem_base;

extern char theme_count;
char theme_name[32];

int sleep_array[] = { 0, 1*60*60, 3*60*60, 5*60*60, 10*60*60, 30*60*60 };
int sleep_time;
int screensaver_count;
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
TextBar *MessageTitle;
TextFlow *MessageTxt;
ListView *MessageList;
tbox *MessageBox;

Window *DialogWin;
TextBar *DialogTitle;
TextFlow *DialogTxt;
tbox *DialogBox;

Typeface *JPGViewerTypeface;

Typeface *TextNormal;
Typeface *TextBold;
Typeface *TextEmph;
Typeface *TextBig;

FILE *config_fp;

const char *pathname[5] = {"PLUGINS", "THEMES", "FONTS", "BITMAPS", "SCREENSAVERS"};
char *path[5] = { NULL, NULL, NULL, NULL, NULL };

int sram_game_size = 64;

int new_marked = -1;
int16 marked;

const char *PogoVersion = "2.0b3mod5";

// Takes a lot of bloody space but I have no better solution currently
//DirList dirlist[MAX_FILE_COUNT];
//char dirsize[MAX_FILE_COUNT][10];
//char dirname[MAX_FILE_COUNT][32];
DirList *dirlist;
// = (DirList *) 0x02000000;
char *dirsize;
// = (char *) (0x02000000+(sizeof(DirList))*MAX_FILE_COUNT);
char *dirname;
// = (char *) (0x02000000+(sizeof(DirList)+10)*MAX_FILE_COUNT);

/* State, saved to  /sram/.state */
struct {
	unsigned /*short*/ char settings[NO_SETTINGS];
	int16 marked;
	uint32 seed;
}  __attribute__ ((packed)) state;

/* Save current state to SRAM */
void save_state(void)
{
	int fd;
	char *name = filesys_get_current();

	memcpy(state.settings, settings, NO_SETTINGS);
	state.marked = marked;
	state.seed = getseed();
	
	fd = open("/sram/.state", O_CREAT);
	if(fd >= 0)
	{
		write(fd, &state, sizeof(state));
		write(fd, name, strlen(name)+1);
		close(fd);
	}

}

/* Load current state from SRAM */
int load_state(void)
{
	char tmp[128];
	int fd;

	fd = open("/sram/.state", 0);
	if(fd >= 0)
	{
		read(fd, &state, sizeof(state));
		read(fd, tmp, 128);
		close(fd);
		filesys_cd_marked(tmp);
		new_marked = state.marked;
		srand(state.seed);
		memcpy(settings, state.settings, NO_SETTINGS);
		
		sleep_time = sleep_array[settings_get(SF_SLEEP)];
		if (settings_get(SF_THEME) >= theme_count)
			settings_set(SF_THEME, 0);
		get_theme_name(settings_get(SF_THEME), theme_name);
		return 1;
	}

	settings_default();
	filesys_cd_marked("");
	new_marked = 0;
	sleep_time = sleep_array[settings_get(SF_SLEEP)];
	get_theme_name(settings_get(SF_THEME), theme_name);

	return 0;
}

int save_user(void)
{
	int fd;
	uchar usr;

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

	return 0;
}

int load_user(void)
{
	int fd;
	uchar usr;

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

	return 0;
}

#define MAXCLIPSIZE (192*1024)
char *clipboard;
char clipname[36];
int clipsize = 0;

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

		if(l > MAXCLIPSIZE)
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
	char dest[128], *current;
	int fd;

	if(!clipsize)
		return -3;

	if(!name)
	{
		name = basename(clipname);
	}

	if (CurrentUser)
		sprintf(dest, "/sram/%s", name);
	else {
		current = filesys_get_current();
		if (!strncmp(current, "/sram/", 6)) {
			strcpy(dest, current);
			strcat(dest,"/");
			strcat(dest, name);
		} else
			sprintf(dest, "/sram/%s", name);
	}

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
char statusbar_buffer[50];
char statusbar_postrtc[60];

void statusbar_set(char *text)
{
	if(StatusBar)
	{
		if (text)
			strcpy(statusbar_buffer, text);
		strcpy(statusbar_postrtc, statusbar_buffer);

		if(rtc_check())
		{
			sprintf(&statusbar_postrtc[strlen(statusbar_postrtc)], "| %02d:%02d", clockdata.tm_hour, clockdata.tm_min);
		}
		textbar_set_attribute(StatusBar, WATR_TEXT, statusbar_postrtc);
	}
}


void statusbar_refresh(void)
{
	statusbar_set(NULL);
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

//BitMap BackgroundBM;

void update_list(void)
{
	char tmp[80];
	int i, t;
	int filecount;

	listview_clear(MainList);
	if (!DialogBox)
		listview_addline(MainList, NULL, NULL, TEXT(PLEASE_WAIT), "");
	screen_redraw(MainScreen);
	filecount = filesys_getfiles(dirlist);
	if (!DialogBox)
		listview_clear(MainList);
	i = 0;
	for(i=0; i<filecount; i++)
	{
		if(dirlist[i].type || settings_get(SF_HIDESIZE) || (filesys_getstate() == FSTATE_GAMES))
			dirsize[i*10] = 0;
		else
			sprintf(&dirsize[i*10], "%d", dirlist[i].entry.d_size);

		t = filetype_lookup(&dirlist[i]);
/*
		//fprintf(stderr, "%s\n", dirlist[i].entry.d_name);
		if(strcmp(dirlist[i].entry.d_name, ".background.bm") == 0)
		{
			FILE *fp = fopen(filesys_fullname(i), "rb");
			BitMap *bmap = bitmap_readbm(fp);
			fclose(fp);
			backdrop_set_attribute(MainList->backdrop, WATR_BITMAP, bmap);
		}
*/
		strcpy(&dirname[i*32], dirlist[i].entry.d_name);
		if((t > 1) && settings_get(SF_HIDEEXT))
		{
			char *p = strrchr(&dirname[i*32], '.');
			if(p && p != &dirname[i*32])
				*p = 0;
		}

		listview_addline(MainList, filetype_textcolor(t), filetype_icon(t), &dirname[i*32], &dirsize[i*10]);
	}

	if (new_marked != -1) {
		marked = new_marked;
		new_marked = -1;
	}

	if (filecount) {
		MainList->marked = -1;
		listview_set_marked(MainList, marked);
		if (listview_get_marked(MainList) == -1) {
			listview_set_marked(MainList, 0);
			marked = 0;
		}
	}

	if(filesys_getstate() == FSTATE_SRAM)
		pprintf(tmp, TEXT(TITLEBAR_SRAM));
	else
		pprintf(tmp, TEXT(TITLEBAR_MAIN));

	textbar_set_attribute(TitleBar, WATR_TEXT, tmp);
	//statusbar_refresh();
}


static BitMap **icon_list;

void textreader_set_font(int n, Font *f);
void textreader_set_typeface(int n, Typeface *tf);
void textreader_init(void);

void setup_screen(void)
{
	char tmp[80];
	int i, count, shrt;
	char *theme;
	Widget *root;
	Font *font;
	
	FILE *fp;
	Widget *dbox;
	Widget *mbox;
	Window *win;
	Scrollbar *sb;
	//BitMap *screen = bitmap_getscreen();
	//bitmap_clear(screen, 0xFF00);

	//fprintf(stderr, "get_theme_name(%d, %p);\n", settings_get(SF_THEME), theme_name);
	get_theme_name(settings_get(SF_THEME), theme_name);
	//fprintf(stderr, "Switch to %s(%p) (%d)\n", theme_name, theme_name, settings_get(SF_THEME));

	memory_free_context(THEME);

	config_fp = fopen(".shell/pogo.cfg", "rb");
	if(!config_fp)
		config_fp = fopen(".pogo.cfg", "rb");
	guiparser_readsymbols(config_fp);
	fclose(config_fp);

	pfree();
	clipboard = pmalloc(MAXCLIPSIZE);
	theme = pmalloc(4097);

	pfree();
	clipboard = pmalloc(MAXCLIPSIZE);
	dirlist = pmalloc(sizeof(DirList) * MAX_FILE_COUNT);
	dirsize = pmalloc(10 * MAX_FILE_COUNT);
	dirname = pmalloc(32 * MAX_FILE_COUNT);

	IconSet = NULL;
	MessageWin = NULL;
	DialogWin = NULL;
	jpgviewer_init();
	textreader_init();

	MainScreen = screen_new();
	win = window_new(MainScreen, 0, 0, 240, 160);

	strcpy(tmp, GET_PATH(THEMES));
	strcat(tmp, theme_name);
	//strcat(tmp, "default.theme");
	//fp = fopen(".shell/themes/default.theme", "rb");
	fp = fopen(tmp, "rb");
	i = fread(theme, 4096, 1, fp);
	theme[i] = 0;
	fclose(fp);

	root = guiparser_create(theme, "root");

	mbox = guiparser_findwidget("msgbox");
	dbox = guiparser_findwidget("dlgbox");

	StatusBar = (TextBar *)guiparser_findwidget("status");
	TitleBar = (TextBar *)guiparser_findwidget("title");
	MainList = (ListView *)guiparser_findwidget("list");
	MessageTxt = (TextFlow *)guiparser_findwidget("mflow");
	MessageTitle = (TextBar *)guiparser_findwidget("mtitle");
	DialogTxt = (TextFlow *)guiparser_findwidget("dflow");
	DialogTitle = (TextBar *)guiparser_findwidget("dtitle");

	JPGViewerTypeface = (Typeface *)guiparser_findwidget("jpgviewer");

	TextNormal = (Typeface *)guiparser_findwidget("textnormal");
	TextBold = (Typeface *)guiparser_findwidget("textbold");
	TextEmph = (Typeface *)guiparser_findwidget("textemph");
	TextBig = (Typeface *)guiparser_findwidget("textbig");

	if(dbox && dbox->type == WIDGET_TRICONTAINER &&
		DialogTxt && DialogTxt->w.type == WIDGET_TEXTFLOW &&
		((TriContainer *)dbox)->children[1] == (Widget *) DialogTxt)
	{
		DialogWin = window_new(MainScreen, 50, 50, 140, 60);
		window_setwidget(DialogWin, dbox);
		window_hide(DialogWin);
		DialogBox = malloc(sizeof(tbox));
		DialogBox->win = DialogWin;
		DialogBox->title = DialogTitle;
		DialogBox->txt = DialogTxt;
		DialogBox->list = NULL;
	}

	if(mbox && mbox->type == WIDGET_TRICONTAINER &&
		MessageTxt && MessageTxt->w.type == WIDGET_TEXTFLOW &&
		((TriContainer *)mbox)->children[1] == (Widget *) MessageTxt)
	{
		//textflow_set_attribute(MessageTxt, WATR_TEXT, "This is just a little\ntest of the messagebox,\nI hope it works well!\n--Sasq");

		MessageWin = window_new(MainScreen, 50, 50, 140, 60);
		//fprintf(stderr, "msgwin %p mbox %p\n", MessageWin, mbox);
		window_setwidget(MessageWin, mbox);
		window_hide(MessageWin);
		MessageList = listview_new_typeface(1, 10, MessageTxt->typeface);
		shrt = MessageTxt->marginlistx;
		listview_set_attribute(MessageList, WATR_MARGIN, (void *) shrt);
		shrt = MessageTxt->marginlisty;
		listview_set_attribute(MessageList, WATR_MARGIN+1, (void *) shrt);

		if(MessageTxt->backdrop)
			listview_set_attribute(MessageList, WATR_BACKDROP, MessageTxt->backdrop);
		listview_set_attribute(MessageList, WATR_COLOR+0, &(MessageTxt->textcolor[0]));
		listview_set_attribute(MessageList, WATR_COLOR+2, &(MessageTxt->textcolor[2]));
		listview_set_attribute(MessageList, WATR_COLOR+3, &(MessageTxt->textcolor[3]));
		shrt = MessageTxt->align[1];
		listview_set_attribute(MessageList, WATR_ALIGN, (void *) shrt);
		MessageBox = malloc(sizeof(tbox));
		MessageBox->win = MessageWin;
		MessageBox->title = MessageTitle;
		MessageBox->txt = MessageTxt;
		MessageBox->list = MessageList;
	}

	if (MainList->scrollbar) {
		sb = MainList->scrollbar;
		if (sb->troughtopbutton && sb->troughtopbutton->bitmap)
			sb->troughtopbutton->bitmap->format |= TRANSPARENT;
		if (sb->troughbottombutton && sb->troughbottombutton->bitmap)
			sb->troughbottombutton->bitmap->format |= TRANSPARENT;
	}

	if (IconSet) {
		count = IconSet->height / IconHeight;

		icon_list = malloc(count * 4);

		for(i=0; i<count; i++)
		{
			icon_list[i] = bitmap_new(IconSet->width, IconHeight, BPP16 | TRANSPARENT | DONT_ALLOC);
			icon_list[i]->pixels = (uint16 *)IconSet->pixels + IconSet->width * i * IconHeight;
		}
	} else
		icon_list = NULL;

	filetype_set_iconset(icon_list);
	filetype_set_icons();

	if (JPGViewerTypeface)
		jpgviewer_set_typeface(JPGViewerTypeface);
	else
		jpgviewer_set_font(font_load_path("cnokia.font"));

	//font->flags |= FFLG_TRANSP;

	if (TextNormal)
		textreader_set_typeface(FONT_TEXT, TextNormal);
	else
		textreader_set_font(FONT_TEXT, font_load_path("dungeon.font"));

	if (TextBold)
		textreader_set_typeface(FONT_TEXT, TextBold);
	else {
		font = font_load_path("dungeon.font");
		font->flags |= FFLG_BOLD;
		textreader_set_font(FONT_BOLD, font);
	}
	
	if (TextEmph)
		textreader_set_typeface(FONT_TEXT, TextEmph);
	else
		textreader_set_font(FONT_EMPH, font_load_path("dungeoni.font"));

	if (TextBig)
		textreader_set_typeface(FONT_TEXT, TextBig);
	else
		textreader_set_font(FONT_BIG, font_load_path("dungeon12.font"));

	window_setwidget(win, root);

	update_list();
}

void cmd_about(char *dummy)
{
	msgbox_info(MessageBox, "About PogoShell 2.0 Beta", "PogoShell v2.0 (Beta 3 mod5 release)\nCreated by Sasq in 2003/2004\nAltered by Kuwanger in 2006\nPress SELECT for help\nat anytime. Enjoy!");
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
	if(msgbox_yesno(MessageBox, tmp) == 1)
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
	int old_theme, old_user;
	char tmp[50];

	old_user = CurrentUser;
	old_theme = settings_get(SF_THEME);
	save_state();
	switch_user();

	if (old_user != CurrentUser) {
		save_user();
		load_state();
		setup_screen();
		sprintf(tmp, TEXT(SWITCHED_USER), UserName);
		statusbar_set(tmp);
	}
	update_list();
}

void cmd_help(char *name)
{
	int i = filesys_getstate();
	switch(i)
	{
	case FSTATE_NORMAL:
		msgbox_info(MessageBox, TEXT(MAIN_HELP_TITLE), TEXT(MAIN_HELP_TEXT));
		break;
	case FSTATE_SRAM:
	default:
		msgbox_info(MessageBox, TEXT(SRAM_HELP_TITLE), TEXT(SRAM_HELP_TEXT));
		break;
	}
}

void cmd_settings(char *name)
{
	int old_hidedot, old_theme;

	old_theme = settings_get(SF_THEME);
	old_hidedot = settings_get(SF_HIDEDOT);
	settings_edit();
	sleep_time = sleep_array[settings_get(SF_SLEEP)];
	if (settings_get(SF_HIDEDOT) != old_hidedot)
		filesys_cd_marked_current();
	save_state();
	if (settings_get(SF_THEME) != old_theme)
		setup_screen();
	//reset_gba();
	update_list();
}

typedef void (*CmdFunc)(char *);

CmdFunc commands[] = { cmd_settings, cmd_about, cmd_switchuser, cmd_help, cmd_sramcopy, cmd_srampaste, cmd_sramdel };


void suspend(void);
void Halt(void);

static int qualifiers = 0;
int show_text(char *cmd, char *fname, int keys);
int vkey_get_qualifiers(void);

extern char *sramfile_mem;
extern int savebank;
extern int sram_size;
int sram_strcmp(const char *a, const char *b);

char main_tmp[50];

int abs(int x)
{
	return (x < 0) ? -x : x;
}

int main(int argc, char **argv)
{
	FILE *fp;
	int c, i;
	int converted = 0;
	int have_state;
	int count = 0;
	int sleepcount = 0;
	int srsize = -1;
	//uint32 *mem = (uint32 *)0x02000000;

	SoftReset(0xfc); //Reset sound, sio, and other registers
	SETW(REG_BG0HOFS, 0x0);
	SETW(REG_BG0VOFS, 0x0);
	SETW(REG_BG1HOFS, 0x0);
	SETW(REG_BG1VOFS, 0x0);
	SETW(REG_BG2HOFS, 0x0);
	SETW(REG_BG2VOFS, 0x0);
	SETW(REG_BG3HOFS, 0x0);
	SETW(REG_BG3VOFS, 0x0);

	/*
	dirlist = pmalloc(sizeof(DirList) * MAX_FILE_COUNT);
	dirsize = pmalloc(10 * MAX_FILE_COUNT);
	dirname = pmalloc(32 * MAX_FILE_COUNT);
	*/

	rtc_enable();
	i = rtc_check();
	rtc_get(main_tmp);

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
		while((c = read_line(main_tmp, sizeof(main_tmp), config_fp)) >= 0)
		{
			if(c > 0)
			{
				if(parse_assignment(main_tmp, &name, &val))
				{
					if(strcmp(name, "SRAM") == 0)
					{
						srsize = atoi(val);
					}
					else
					if(strcmp(name, "SAVEBANK") == 0)
					{
						savebank = atoi(val);
					}
					else
					/*if(strcmp(name, "GAME") == 0)
					{
						sram_game_size = atoi(val);
					}
					else*/
					for(i=0; i<5; i++)
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
		sramfile_mem = (char *)0x0E010000 + (savebank * 0x10000);
	}

	converted = sram_convert();

	sram_init();

	sram_fd = open("/sram", 0);

	if (GET_PATH(SCREENSAVERS)) {
		DIR *dir;
		struct dirent *de;

		dir = opendir(GET_PATH(SCREENSAVERS));
		if (dir) {
			while ((de = readdir(dir))) {
				if (!(de->d_size & 0x80000000))
					screensaver_count++;
			}
			closedir(dir);
		}
	}

	filesys_cd_marked("");

	read_texts(config_fp);
	read_users(config_fp);

	filetype_readtypes(config_fp);

	fclose(config_fp);

	settings_init();
	//strcpy(theme_name, "default.theme");

	load_user();
	have_state = load_state();

	//dprint("blah\n");
	//time(NULL);

	//settings_read();

	fp = fopen(".shell/splash.bmp", "rb");
	fclose(fp);

	if(fp)
	{
		bmp_view(".shell/splash.bmp");
	}

	memory_set_context(THEME);

	setup_screen();

	if((fp = fopen("/sram/.bookmark", "rb")))
	{
		unsigned int l;
		fread(&l, 1, 4, fp);
		if(l & 0x80000000)
		{
			l &= 0x7FFFFFFF;
			fread(main_tmp, 1, sizeof(main_tmp), fp);
			fclose(fp);
			show_text("", main_tmp, 0);
		}
	}

	if (!DialogBox)
		listview_addline(MainList, NULL, NULL, TEXT(PLEASE_WAIT), "");
	screen_redraw(MainScreen);

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
		save_user();
	}

	savesys_savelastgame();

	if (!DialogBox)
		listview_clear(MainList);

	screen_redraw(MainScreen);

	while(1)
	{
		int h;
		marked = listview_get_marked(MainList);
		sleepcount = count = 0;
		while((c = getchar()) == EOF)
		{
			count++;
			sleepcount++;
			Halt();

			if (sleep_time && sleepcount >= sleep_time) {
				if (settings_get(SF_SCREENSAVER) && screensaver_count) {
					DIR *dir;
					struct dirent *result;
					int i;
					DirList dl;
					char filename[128];

					i = abs(rand()) % screensaver_count;
					dir = opendir(GET_PATH(SCREENSAVERS));
					do {
						if (readdir_r(dir, &dl.entry, &result) || !result) {
							i = -2;
							break;
						}
						if (!(dl.entry.d_size & 0x80000000))
							i--;
					} while (i >= 0);
					closedir(dir);
					if (i != -2) {
						//strcpy(filename, "/rom/");
						strcpy(filename, GET_PATH(SCREENSAVERS));
						strcat(filename, dl.entry.d_name);
						dl.type = 0;
						i = filetype_lookup(&dl);
						if (filetype_handle(filename, i, 0) == 2)
							update_list();
					}
				} else { 
					suspend();
					getchar(); //dump char used for wakeup
				}
				sleepcount = 0;
				count = 50;
			}

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
			if(qualifiers == 2) {
				h = listview_get_marked(MainList);
				listview_set_marked(MainList, marked - MainList->showing);
				if (h == listview_get_marked(MainList))
					listview_set_marked(MainList, 0);
			}
			else
				listview_set_marked(MainList, marked-1);
			break;

		case RAWKEY_DOWN:
			filesys_update_key(0x1);
			if(qualifiers == 1)
				listview_set_marked(MainList, MainList->lines-1);
			else
			if(qualifiers == 2) {
				h = listview_get_marked(MainList);
				listview_set_marked(MainList, marked + MainList->showing);
				if (h == listview_get_marked(MainList))
					listview_set_marked(MainList, MainList->lines-1);
			} else
				listview_set_marked(MainList, marked+1);
			break;

		case RAWKEY_A:
			{
				char *name = filesys_fullname(marked);
				if(filesys_getstate() == FSTATE_SRAM)
				{
					switch(qualifiers)
					{
					case 0:
						i = filetype_lookup(&dirlist[marked]);
						if(filetype_handle(name, i, 0) == 2)
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
					if (filetype_handle(name, i, qualifiers) == 2)
						update_list();
				}
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
				strcpy(main_tmp, TEXT(CMD_BASIC));
				if(filesys_getstate() == FSTATE_SRAM)
				{
					strcat(main_tmp, TEXT(CMD_SRAM));
					i += 2;
				}
				i = msgbox_list2(MessageBox, TEXT(COMMANDS), main_tmp, i);

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

