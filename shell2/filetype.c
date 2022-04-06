#include <pogo.h>

#include "gba_defs.h"

#include "bitmap.h"
#include "filesys.h"
#include "filetype.h"
#include "savesystem.h"
#include "window.h"
#include "widgets/listview.h"
#include "widgets/textbar.h"
#include "settings.h"
#include "misc.h"
#include "text.h"

extern int savebank;
extern int clipsize;
extern uint16 marked;

int execute_mb_joined(char *fname, int decompression, int keys);
void bm_view(char *fname);
void bmp_view(char *fname);
void bmz_view(char *fname);
void bmap_view(char *fname);
void jpg_view(char *fname);
void jpe_view(char *fname);
//extern void reset(void);
extern void setup_screen(void);

static BitMap **icon_set;
static BitMap *settings_icn = NULL;
static int settings_iconidx = -1;

static int ftcount = 1;
static FileType *filetypes[100];

extern ListView *MainList;
//extern TextBar *StatusBar;
void statusbar_set(char *);
extern Screen *MainScreen;

extern void LZ77UnCompWram(void *Srcp, void *Destp);
extern void SoftReset(unsigned char flags);

void save_state(void);


static int check_dir(char *data, DirList *entry)
{
	return entry->type == 1;
}

static int check_dev(char *data, DirList *entry)
{
	//fprintf(stderr, "Testing %s %s\n", data, entry->name);
	return entry->type == 2;
}

static int check_extention(char *data, DirList *entry)
{
	char *p = strrchr(entry->entry.d_name, '.');
	if(p)
	{
		p++;
		return (strcmp(p, data) == 0);
	}

	return 0;
}

/*void clear_vram(void)
{
	unsigned short *vram = (unsigned short *) VRAM;
	int i;

	// Start after screen.
	for (i = 240*160; i < (VRAM_END-VRAM)/sizeof(unsigned short); i++)
		vram[i] = 0;
}*/

int execute_mb(char *cmd, char *fname, int keys)
{
	return execute_mb_joined(fname, RAW, keys);
}

int execute_mbz(char *cmd, char *fname, int keys)
{
	return execute_mb_joined(fname, LZ77, keys);
}

int execute_mbap(char *cmd, char *fname, int keys)
{
	return execute_mb_joined(fname, APACK, keys);
}

int execute_mb_joined(char *fname, int decompression, int keys)
{
	char *ptr;
	uint16 *p2;
	int i;

	save_state();

	memset((void *)(0x02000000), 0, 256*1024);
	ptr = file2mem(fname, (void *)0x02000000, 256*1024, decompression);
	if (ptr != (void *)0x02000000)
		return 2;

	set_ram_start(savebank);

	SETW(REG_IE, 0);
	SETW(REG_IF, 0);
	SETW(REG_IME, 0);
	SETW(REG_SOUNDBIAS, 0x0200);
	//SETW(REG_DISPCNT, DISP_MODE_0 | DISP_BG1_ON );
	//SETW(REG_BG1CNT, 0);
	// Don't use memset as memset is in iwram
	p2 = (uint16 *) 0x03000000;
	for (i = 0; i < 0x8000/2; i++)
		p2[i] = 0;
	//SoftReset(0xe2);
	//SoftReset(0xfe);
	((void(*)(void))0x02000000)();

	return 2;

}

int execute_rom(char *cmd, char *fname, int keys)
{
	const char *args[] = {NULL, NULL};

	save_state();
	//clear_vram();

	savesys_handleexec(fname);

	set_ram_start(savebank);
	//reset_io();
	if(keys == 2)
		execv_jump(fname, args, (void *)0x0000008C);
	else
	if(keys == 1 || settings_get(SF_INTROSKIP))
		execv_jump(fname, args, (void *)0x080000C0);
	else
		execv(fname, args);
	return 1;

}

void make_arguments(const char *cmdname, const char *const *argv);

int execute_plugin(char *cmd, char *fname, int keys)
{
	int i;
	int plugin_type;
	char *s;
	char tmp[64];
	const char *args[] = {NULL, NULL};
	args[0] = fname;

	s = strrchr(cmd, '.');
	if (!s) {
		statusbar_set(TEXT(INVALID_PLUGIN));
		screen_redraw(MainScreen);
		return 1;
	}

	s++;

	for(i=1; i<ftcount; i++)
	{
		if(strcmp(s,filetypes[i]->data) == 0)
			break;
	}

	if (i >= ftcount) {
		statusbar_set(TEXT(INVALID_PLUGIN));
		screen_redraw(MainScreen);
		return 1;
	}

	if (filetypes[i]->handle_func == execute_mb)
		plugin_type = RAW;
	else if (filetypes[i]->handle_func == execute_mbz)
		plugin_type = LZ77;
	else if (filetypes[i]->handle_func == execute_mbap)
		plugin_type = APACK;
	else if (filetypes[i]->handle_func == execute_rom)
		plugin_type = EXE;
	else {
		statusbar_set(TEXT(INVALID_PLUGIN));
		screen_redraw(MainScreen);
		return 1;
	}

	save_state();
	//clear_vram();

	//sprintf(tmp, ".shell/plugins/%s", cmd);
	strcpy(tmp, GET_PATH(PLUGINS));
	strcat(tmp, cmd);

	if(keys == 1)
		savesys_handleexec(fname);
	else
	if(keys == 2)
		savesys_handleexec(cmd);

	//if(keys)
	//	reset_io();

	SETW(REG_IE, 0);
	SETW(REG_IF, 0);
	SETW(REG_IME, 0);
	SETW(REG_SOUNDBIAS, 0x0200);
		
	if (plugin_type != EXE) {
		char *ptr;
		uint16 *p2;

		memset((void *)(0x02000000), 0, 256*1024);
		ptr = file2mem(tmp, (void *)0x02000000, 256*1024, plugin_type);
		if(ptr != (char *) 0x02000000)
			return 1;
		make_arguments(tmp, args);

		//fprintf(stderr, "Copied from %p\n", ptr);

		set_ram_start(savebank);

		// Don't use memset as memset is in iwram
		p2 = (uint16 *) 0x03000000;
		for (i = 0; i < 0x8000/2; i++)
			p2[i] = 0;
		//SoftReset(0xe2);
		((void(*)(void))0x02000000)();
	}

	set_ram_start(savebank);
	execv(tmp, args);
	return 1;
}

int set_font(char *cmd, char *fname, int keys)
{
	Typeface *tf, *typeface;

	tf = listview_get_typeface(MainList);
	typeface = typeface_new(font_load(fname), 0);

	typeface_set_attribute(typeface, WATR_STYLE, (void *) FFLG_TRANSP);
	typeface_set_attribute(typeface, WATR_STYLE, (void *) tf->style);
	typeface_set_attribute(typeface, WATR_COLOR + 0, &(tf->shadow));
	typeface_set_attribute(typeface, WATR_COLOR + 1, &(tf->outline));

	listview_set_attribute(MainList, WATR_TYPEFACE, typeface);
	return 1;
}

int set_theme(char *cmd, char *fname, int keys)
{
	char *s;

	s = fname;
	while (*s) s++;
	while (*s != '/') s--;

	if (strlen(GET_PATH(THEMES)) == (s - fname - 4) && !strncmp(&fname[5], GET_PATH(THEMES), s - fname - 4))
	{
		if (!set_theme_setting(s+1)) {
			save_state();
			setup_screen();
		}
	}
	return 1;
}

int changedir(char *cmd, char *fname, int keys)
{
	filesys_cd(fname, marked);
	return 2;
}

int do_nothing(char *cmd, char *fname, int keys)
{
	return 1;
}

int textreader_show_text(char *name);

int show_text(char *cmd, char *fname, int keys)
{
	textreader_show_text(fname);
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 1;
}

int showbm(char *cmd, char *fname, int keys)
{
	bm_view(fname);
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showbmp(char *cmd, char *fname, int keys)
{
	bmp_view(fname);
	clipsize = 0;
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showbmz(char *cmd, char *fname, int keys)
{
	bmz_view(fname);
	clipsize = 0;
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showbmap(char *cmd, char *fname, int keys)
{
	bmap_view(fname);
	clipsize = 0;
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showjpe(char *cmd, char *fname, int keys)
{
	jpe_view(fname);
	clipsize = 0;
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showjpg(char *cmd, char *fname, int keys)
{
	jpg_view(fname);
	clipsize = 0;
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

/*
void filetype_register(char *ext, BitMap *bm, int handler)
{
	FileType *ft;
	
	ft = filetypes[ftcount++] = malloc(sizeof(FileType));

	strcpy(ft->data, ext);
	if(handler == 0)
		ft->compare_func = check_extention;
	else
		ft->compare_func = check_dir;
	ft->icon = bm;
	ft->handle_func = NULL;

	if(!(ext && strlen(ext)))
		default_type = ftcount-1;
}
*/

void filetype_readtypes(FILE *fp)
{
	char buf[80];
	int i, rc;
	FileType *f;
	char *p, *ptr;

	if(find_section(fp, "filetypes"))
	{
		/* Every line is one filetype */
		while((rc = read_line(buf, sizeof(buf), fp)) >= 0)
		{
			if(rc > 0)
			{
				f = filetypes[ftcount++] = malloc(sizeof(FileType));
				ptr = buf;
				while(*ptr++ != ' ');

				f->desc = NULL;
				f->command = NULL;
				f->compare_func = check_extention;
				f->handle_func = NULL;
				f->textcolor = NULL;
				f->saver = 0;

				ptr[-1] = 0;

				/* First word is the extention */
				strcpy(f->data, buf);
				//fprintf(stderr, "EXT:%s\n", buf);

				/* Second word is the icon */
				if(isdigit(*ptr))
				{
					i = atoi(ptr);
					//fprintf(stderr, "ICON:%d\n", i);

					f->iconidx = i;
					f->icon = NULL;
					while(isdigit(*ptr++));
				}
				else
				{
					//FILE *fp2;
					p = ptr;
					while(!isspace(*ptr)) ptr++;
					*ptr++ = 0;
					//fp2 = fopen(p, "rb");
					//f->icon = bitmap_readbm(fp2);
					//fclose(fp2);
					f->iconidx = -1;
					f->icon = bitmap_loadbm(p);
				}

				p = ptr;

				/* Third word is the handler */
				while((*ptr != ' ') && (*ptr != 10) && (*ptr != 13)) ptr++;
				if(*ptr == ' ')
				{
					char *q = ptr+1;

					if(isdigit(*q))
					{
						f->saver = atoi(q);
						//fprintf(stderr, "SAVER:%d\n", f->saver);
						while(isdigit(*q++));
						*ptr = 0;
						ptr = q - 1;
					}

					if(*ptr == ' ' && *(ptr+1) == '$')
					{
						*ptr = 0;
						ptr += 2;
						i = gethex(ptr);
						while (isalnum(*ptr++));
						q = ptr--;
						f->textcolor = malloc(sizeof(Color));
						f->textcolor->r = (i>>16) & 0xff;
						f->textcolor->g = (i>>8) & 0xff;
						f->textcolor->b = i & 0xff;
						f->textcolor->a = 0;
					}

					if (*q && *q != 10 && *q != 13) {
						/* The rest of the line is the description, if it exists */
						while ((*q != 10) && (*q != 13)) q++;
						*q = 0;
						f->desc = malloc(strlen(ptr+1)+1);
						strcpy(f->desc, ptr+1);
						//fprintf(stderr, "DESC:%s\n", ptr+1);
					}
				}
				*ptr = 0;
				/* Set correct internal handler for filetype */

				if(strcmp(p, "EXE") == 0)
					f->handle_func = execute_rom;
				else
				if(strcmp(p, "DIR") == 0)
				{
					f->compare_func = check_dir;
					f->handle_func = changedir;
				}
				else
				if(strcmp(p, "SAV") == 0)
				{
					f->handle_func = do_nothing;
				}
				else
				if(strcmp(p, "DEF") == 0)
				{	
					filetypes[0] = f;
					ftcount--;
				}
				else
				if(strcmp(p, "SET") == 0)
				{
					settings_icn = f->icon;
					settings_iconidx = f->iconidx;
					ftcount--;
				}
				else
				if(strcmp(p, "DEV") == 0)
				{	
					f->compare_func = check_dev;
					f->handle_func = changedir;
				}
				else
				if(strcmp(p, "TXT") == 0)
					f->handle_func = show_text;
				else
				if(strcmp(p, "MB") == 0)
					f->handle_func = execute_mb;
				else
				if(strcmp(p, "MBZ") == 0)
					f->handle_func = execute_mbz;
				else
				if(strcmp(p, "MBAP") == 0)
					f->handle_func = execute_mbap;
				else
				if(strcmp(p, "THM") == 0)
					f->handle_func = set_theme;
				else
				if(strcmp(p, "FNT") == 0)
					f->handle_func = set_font;
				else
				if(strcmp(p, "BM") == 0)
					f->handle_func = showbm;
				else
				if(strcmp(p, "BMP") == 0)
					f->handle_func = showbmp;
				else
				if(strcmp(p, "BMZ") == 0)
					f->handle_func = showbmz;
				else
				if(strcmp(p, "BMAP") == 0)
					f->handle_func = showbmap;
				else
				if(strcmp(p, "JPE") == 0)
					f->handle_func = showjpe;
				else
				if(strcmp(p, "JPG") == 0)
					f->handle_func = showjpg;
				else
				{
					/* No internal filetype found so assume handler is an actual rom that will be
					executed for files with this extention */
					f->handle_func = execute_plugin;
					f->command = malloc(strlen(p)+1);
					strcpy(f->command, p);
					//fprintf(stderr, "PLUGIN:%s\n", p);

				}
			}
		}
	}
}

void filetype_set_iconset(BitMap **icons)
{
	icon_set = icons;
}

void filetype_set_icons(void)
{
	FileType *f;
	int i;
	if (icon_set) {
		for (i=0; i<ftcount; i++)
		{
			f = filetypes[i];
		
			if (f->iconidx >= 0)
				f->icon = icon_set[f->iconidx];
		}
		if (settings_iconidx >= 0) {
			settings_icn = icon_set[settings_iconidx];
		}
	}
	settings_icon(settings_icn);		
}

int filetype_font(DirList *entry)
{
	return (filetypes[filetype_lookup(entry)]->handle_func == set_font);
}

int filetype_bm(DirList *entry)
{
	return (filetypes[filetype_lookup(entry)]->handle_func == showbm);
}

int filetype_theme(DirList *entry)
{
	return (filetypes[filetype_lookup(entry)]->handle_func == set_theme);
}

int filetype_lookup(DirList *entry)
{
	int i;
	for(i=1; i<ftcount; i++)
	{
		if(filetypes[i]->compare_func(filetypes[i]->data, entry))
			return i;
	}

	return 0;
}

int filetype_handle(char *fname, int type, int keys)
{
	if(!type)
		return 0;

	//fprintf(stderr, "Handling '%s' type %d\n", fname, type);
	if(filetypes[type]->desc)
	{
		//textbar_set_attribute(StatusBar, WATR_TEXT, filetypes[type]->desc);
		statusbar_set(filetypes[type]->desc);
		screen_redraw(MainScreen);
	}
	return filetypes[type]->handle_func(filetypes[type]->command, fname, keys | filetypes[type]->saver);
}


BitMap *filetype_icon(int type)
{
	return filetypes[type]->icon;
}

Color *filetype_textcolor(int type)
{
	return filetypes[type]->textcolor;
}
