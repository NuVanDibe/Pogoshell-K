#ifndef FILETYPE_H
#define FILETYPE_H

#include "bitmap.h"
#include "filesys.h"

typedef struct
{
	int iconidx;
	BitMap *icon;
	Color *textcolor;
	int (*handle_func)(char *data, char *fname, int keys);
	char data[8];
	int (*compare_func)(char *data, DirList *entry);
	char *desc;
	char *command;
	uchar saver;

} FileType;

void filetype_register(char *ext, BitMap *bm, int handler);
int filetype_bm(DirList *entry);
int filetype_font(DirList *entry);
int filetype_theme(DirList *entry);
int filetype_lookup(DirList *entry);
int filetype_handle(char *fname, int type, int keys);

BitMap *filetype_icon(int type);
Color *filetype_textcolor(int type);
void filetype_readtypes(FILE *fp);
void filetype_set_iconset(BitMap **icons);
void filetype_set_icons(void);

#endif
