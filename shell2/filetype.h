#ifndef FILETYPE_H
#define FILETYPE_H

#include "bitmap.h"
#include "filesys.h"

typedef struct
{
	BitMap *icon;
	int (*handle_func)(char *data, char *fname, int keys);
	char data[8];
	int (*compare_func)(char *data, DirList *entry);
	char *desc;
	char *command;
	uchar saver;

} FileType;

void filetype_register(char *ext, BitMap *bm, int handler);
int filetype_lookup(DirList *entry);
int filetype_handle(char *fname, int type, int keys);

BitMap *filetype_icon(int type);
void filetype_readtypes(FILE *fp);
void filetype_set_iconset(BitMap **icons);

#endif
