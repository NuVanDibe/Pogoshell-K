#include <pogo.h>

#include "gba_defs.h"

#include "bitmap.h"
#include "filesys.h"
#include "filetype.h"
#include "savesystem.h"
#include "freespace.h"
#include "window.h"
#include "widgets/listview.h"
#include "widgets/textbar.h"
#include "settings.h"
#include "misc.h" 

extern uint16 marked;

void bmp_view(char *fname);
void jpg_view(char *fname);
void jpe_view(char *fname);

static BitMap **icon_set;

static int ftcount = 1;
static FileType *filetypes[50];

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
	char *p = strrchr(entry->name, '.');
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
	uchar *ptr;
	uint16 *p2;
	int i;
	//int fd = open(fname, 0);
	//void *p = (void *)lseek(fd, 0, SEEK_MEM);
	//close(fd);

	save_state();
	//clear_vram();
	
	//set_ram_start(0);

	memset((void *)(0x02000000), 0, 256*1024);
	ptr = file2mem(fname, (void *)0x02000000, 256*1024);
	if(ptr != (void *)0x02000000)
		memcpy((void *)0x02000000, ptr, 256*1024);

	set_ram_start(0);

	//if(fname[strlen(fname)-1] == 'z')
	//	LZ77UnCompWram(p, (void *)0x02000000);
	//else


	SETW(REG_IE, 0);
	SETW(REG_IF, 0);
	SETW(REG_IME, 0);
	SETW(REG_DISPCNT, DISP_MODE_0 | DISP_BG1_ON );
	SETW(REG_BG1CNT, 0);
	SETW(REG_SOUNDBIAS, 0x0200);
	// Don't use memset as memset is in iwram
	p2 = (uint16 *) 0x03000000;
	for (i = 0; i < 0x8000/2; i++)
		p2[i] = 0;
	//SoftReset(0xe2);
	//SoftReset(0xfe);
	((void(*)(void))0x02000000)();

	return 1;

}

int execute_rom(char *cmd, char *fname, int keys)
{
	const char *args[] = {NULL, NULL};

	save_state();
	//clear_vram();

	savesys_handleexec(fname);

	set_ram_start(0);
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
	char *s;
	char tmp[64];
	const char *args[] = {NULL, NULL};
	args[0] = fname;

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

	s = strrchr(cmd, '.');
	if(s && (strncmp(s, ".mb", 3) == 0))
	{
		uchar *ptr;
		uint16 *p2;
		int i;

		memset((void *)(0x02000000), 0, 256*1024);
		ptr = file2mem(tmp, (void *)0x02000000, 256*1024);
		if(ptr != (uchar *) 0x02000000)
			memcpy((void *) 0x02000000, ptr, 256*1024);
		make_arguments(tmp, args);

		//fprintf(stderr, "Copied from %p\n", ptr);

		set_ram_start(0);

		SETW(REG_IE, 0);
		SETW(REG_IF, 0);
		SETW(REG_IME, 0);
		SETW(REG_SOUNDBIAS, 0x0200);
		
		// Don't use memset as memset is in iwram
		p2 = (uint16 *) 0x03000000;
		for (i = 0; i < 0x8000/2; i++)
			p2[i] = 0;
		//SoftReset(0xe2);
		((void(*)(void))0x02000000)();
	}

	set_ram_start(0);
	execv(tmp, args);
	return 1;
}

int set_font(char *cmd, char *fname, int keys)
{
	Font *font = font_load(fname);
	font->flags |= FFLG_TRANSP;
	if(MainList->font)
		free(MainList->font);
	listview_set_attribute(MainList, WATR_FONT, font);
	return 1;
}

int changedir(char *cmd, char *fname, int keys)
{
	filesys_cd(fname, marked);
	return 2;
}

int textreader_show_text(char *name);

int show_text(char *cmd, char *fname, int keys)
{
	textreader_show_text(fname);
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 1;
}

int showbmp(char *cmd, char *fname, int keys)
{
	bmp_view(fname);
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showjpe(char *cmd, char *fname, int keys)
{
	jpe_view(fname);
	MainScreen->firstWindow->widget->flags |= WFLG_REDRAW;
	return 2;
}

int showjpg(char *cmd, char *fname, int keys)
{
	jpg_view(fname);
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

					f->icon = icon_set[i];
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
						ptr = q-1;
					}

					/* The rest of the line is the description, if it exists */
					while((*q != 10) && (*q != 13)) q++;
					*q = 0;
					f->desc = malloc(strlen(ptr+1)+1);
					strcpy(f->desc, ptr+1);
					//fprintf(stderr, "DESC:%s\n", ptr+1);
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
				if(strcmp(p, "DEF") == 0)
				{	
					filetypes[0] = f;
					ftcount--;
				}
				else
				if(strcmp(p, "SET") == 0)
				{	
					settings_icon(f->icon);		
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
				if(strcmp(p, "FNT") == 0)
					f->handle_func = set_font;
				else
				if(strcmp(p, "BMP") == 0)
					f->handle_func = showbmp;
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
