#include "core.h"
#include "device.h"
#include "max_file_count.h"

typedef struct {
	unsigned int size;
	unsigned char csum;
	unsigned char namesize;
	char name[0];
} Romfile;

Romfile *file_tab[MAX_OPENFILE_COUNT];
int file_count = 0;

typedef struct {
	int pos;
	uint16 romfile;
} OpenFile;

static int open_files = 0;
static OpenFile open_filetab[MAX_OPENFILE_COUNT];

extern uint32 __ewram_overlay_lma;


static Device rfdev;

static int rf_open(const char *name, int flags)
{
	int i;
	OpenFile *f;

	if(!*name)
	{
		f = &open_filetab[open_files++];
		f->romfile = 0xFF;
		f->pos = 0;
		return open_files-1;
	}

	for(i=0; i<file_count; i++)
		if(strcmp(name, file_tab[i]->name) == 0)
		{
			f = &open_filetab[open_files++];
			f->romfile = i;
			f->pos = 0;
			return open_files-1;
		}

	return -1;
}

static int rf_read(int fd, void *dest, int size)
{
	Romfile *r;
	OpenFile *f;

	f = &open_filetab[fd];

	if(f->romfile == 0xFF)
	{
		char *d = (char *)dest;
		while(f->pos < file_count && size >= 32)
		{
			r = file_tab[f->pos];
			memset(d, 0, 32);
			strcpy(d, r->name);
			d+=32;
			size-=32;
			f->pos++;
		}
		return d-(char*)dest;
	}

	r = file_tab[f->romfile];

	/* Get ptr to current pos in ROM */
	uchar *p = &((uchar *)r)[r->namesize + 6 + f->pos];
	if(f->pos + size > r->size)
		size = (r->size - f->pos);
	if(size) {
		memcpy(dest, p, size);
		f->pos += size;
	}
	return size;
}

static int rf_close(int fd)
{
	OpenFile *f;

	f = &open_filetab[fd];
	f->pos = -1;

	f = &open_filetab[open_files-1];
	while(open_files && f->pos == -1) {
		open_files--;
		f = &open_filetab[open_files-1];
	}
	return 0;
}

static int rf_lseek(int fd, int offset, int origin)
{
	int newpos = 0;
	Romfile *r;
	OpenFile *f;

	f = &open_filetab[fd];
	r = file_tab[f->romfile];

	switch(origin) {
	case SEEK_SET:
		newpos = offset;
		break;
	case SEEK_CUR:
		newpos = f->pos + offset;
		break;
	case SEEK_END:
		newpos = r->size - offset;
		break;
	case SEEK_MEM:
		return (int)&((uchar *)r)[r->namesize + 6 + f->pos];
		break;
	}

	if(newpos < 0)
		return -1;
	if(newpos > r->size) {
		newpos = r->size;
		//dprint("Seeking beyond end of file\n");
	}

	return f->pos = newpos;
}


void filesys_init(void)
{
	int i;
	Romfile *f;
	uint32 *start = &__ewram_overlay_lma;

	for(i=0; i<4; i++) {
		if((*start == 0xFAB0BABE)) {
			start++;
			while(*start != -1 && file_count < MAX_FILE_COUNT) {
				f = file_tab[file_count++] = (Romfile *)start;
				start += (f->size+f->namesize+6+3)/4;
				//printf("File \"%s\" (%d bytes)\n", f->name, f->size);
			}
			i = 4;
		}
		start++;
	}

	memset(&rfdev, 0, sizeof(rfdev));
	rfdev.open = rf_open;
	rfdev.close = rf_close;
	rfdev.read = rf_read;
	rfdev.lseek = rf_lseek;
	device_register(&rfdev, "/rom/", NULL, -1);
}
