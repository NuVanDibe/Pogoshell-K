/*
 * ROM Filesystem Device (romfilesys.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * A rom filesystem that supports recursive directories. The filesystem 
 * is placed in the cart after the actual program/game rom by a PC
 * program called MAKEFS
 **/

#include "core.h"
#include "device.h"
#include "io.h"
#include "max_file_count.h"
#include "aes.h"

char *rootdir;

Romfile rootfile;

typedef struct {
	int pos;
	Romfile *file;
} OpenFile;

static int open_files = 0;
static OpenFile open_filetab[MAX_OPENFILE_COUNT];

static uint64 current_dir_key[2];

/* Prevent nesting hidden dirs, since we only have room for one key */
static int in_hidden_dir;

/* "Cache" decryption */
static int already_decrypted = 1;

static Romfile hidden_dir[MAX_HIDDEN_DIR+1];

static Device rfdev;

static char *strchr2(const char *str, int c)
{
	while(*str && *str != c) str++;
	if(*str)
		return (char *)str;
	else
		return NULL;
}


static Romfile *findfile(Romfile *rf, char *name, int size)
{
	uint64 *hidden, *start;
	static uint64 old_enc[2], temp_salt[2];
	int i, tmpsize;
	aes_context ctx;

	size &= 0x7FFFFFFF;
	while(size)
	{
		if(strcmp(rf->name, name) == 0)
			return rf;
		rf++;
		size -= sizeof(Romfile);
	}

	if (!in_hidden_dir && strcmp(name, "<hidden>") == 0)
	{
		hidden = (uint64 *) rf;
		temp_salt[0] = hidden[0];
		temp_salt[1] = hidden[1];
		aes_set_key(&ctx, (uchar *) current_dir_key, 16, 0);
		aes_encrypt(&ctx, (uchar *) temp_salt, (uchar *) temp_salt);
		if (temp_salt[0] == hidden[2] &&
			temp_salt[1] == hidden[3])
		{
			if (!already_decrypted) {
				aes_decrypt(&ctx, (uchar *) &hidden[4], (uchar *) temp_salt);
				strcpy(hidden_dir[0].name, "<hidden>");
				size = (unsigned int) (temp_salt[1] >> 32) & 0x7FFFFFFF;
				/* Limit size of hidden dir to keep from running into problems */
				tmpsize = (size + 15) & ~15;
				if (tmpsize > MAX_HIDDEN_DIR*sizeof(Romfile))
					return NULL;
				start = (uint64 *) &rootdir[(int) (temp_salt[0] & 0xFFFFFFFF)];
				old_enc[0] = old_enc[1] = 0xfedcba9876543210ll;
				for (i = 0; i < ((tmpsize+sizeof(uint64)-1)>>1)/sizeof(uint64); i++)
				{
					aes_decrypt(&ctx, (uchar *) &start[(i<<1)], ((uchar *) &hidden_dir[1]) + (i<<4));
					*(uint64 *)((uchar *) &hidden_dir[1] + (i<<4)) ^= old_enc[0];
					*(uint64 *)((uchar *) &hidden_dir[1] + (i<<4) + 8) ^= old_enc[1];
					old_enc[0] = start[(i<<1)];
					old_enc[1] = start[(i<<1)+1];
				}
				hidden_dir[0].start = ((int) &hidden_dir[1]) - ((int) rootdir);
				hidden_dir[0].size = size | 0x80000000;
			}
			in_hidden_dir = 1;
			already_decrypted = 1;
			return &hidden_dir[0];
		}
	}
		
	return NULL;
}

static int rf_open(const char *name, int flags)
{
	char tmp[128];
	int size;
	char *p, *p2;
	OpenFile *f;
	Romfile *rf;

	in_hidden_dir = 0;

	while(*name == '/')
		name++;
	strcpy(tmp, name);

	if(!strlen(name))
	{
		f = &open_filetab[open_files++];
		f->file = &rootfile;
		f->pos = 0;
		return open_files-1;
	}

	p = NULL;
	p = strchr2(tmp, '/');
	if(p)
		*p = 0;

	rf = findfile((Romfile *)rootdir, tmp, rootfile.size);
	if(!rf)
		return -1;
	while(p)
	{
		p++;
		p2 = strchr2(p, '/');
		if(p2)
			*p2 = 0;

		size = rf->size;
		rf = (Romfile *)&rootdir[rf->start];
		rf = findfile(rf, p, size);
		if(!rf)
			return -1;
		p = p2;
	}

	f = &open_filetab[open_files++];
	f->file = rf;
	f->pos = 0;
	return open_files-1;
}


static int rf_read(int fd, void *dest, int size)
{
	Romfile *rf;
	OpenFile *f;
	int rsize;

	f = &open_filetab[fd];
	rf = f->file;
	rsize = rf->size & 0x7fffffff;

	/* Get ptr to current pos in ROM */
	uchar *p = &((uchar *)rootdir)[rf->start + f->pos];
	if(f->pos + size > rsize)
		size = (rsize - f->pos);
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

static int rf_stat(const char *name, struct stat *stat)
{
	Romfile *rf;
	int fd = rf_open(name, 0);
	memset(stat, 0, sizeof(struct stat));
	if(fd >= 0)
	{
		rf = open_filetab[fd].file;

		if(rf->size & 0x80000000)
		{
			stat->st_mode |= S_IFDIR;
		}
		stat->st_size = rf->size & 0x7FFFFFFF;
		stat->st_ino = (int)&rootdir[rf->start];
		open_files--;
		return 0;
	}
	return -1;
}

static int rf_lseek(int fd, int offset, int origin)
{
	int rsize, newpos = 0;
	Romfile *r;
	OpenFile *f;

	f = &open_filetab[fd];
	r = f->file;
	rsize = r->size & 0x7fffffff;

	switch(origin) {
	case SEEK_SET:
		newpos = offset;
		break;
	case SEEK_CUR:
		newpos = f->pos + offset;
		break;
	case SEEK_END:
		newpos = rsize - offset;
		break;
	case SEEK_MEM:
		return (int)&((uchar *)rootdir)[r->start + f->pos];
		break;
	}

	if(newpos < 0)
		return -1;
	if(newpos > rsize) {
		newpos = rsize;
		//dprint("Seeking beyond end of file\n");
	}

	return f->pos = newpos;
}

/* Support various kee operations
   Note: To get the key, one must use getkey1-4 and
         stitch it back together again */
static int rf_ioctl(int fd, int req, va_list vl)
{
	uint64 **getkeyptr, *keyptr;
	static uint64 key_holder[2];
	char *dir;
	int ofd;

	switch(req)
	{
	case RM_GETKEY:
		getkeyptr = va_arg(vl, uint64 **);
		(*getkeyptr)[0] = current_dir_key[0];
		(*getkeyptr)[1] = current_dir_key[1];
		return 0;
		break;
	case RM_SETKEY:
		already_decrypted = 0;
		keyptr = va_arg(vl, uint64 *);
		current_dir_key[0] = keyptr[0];
		current_dir_key[1] = keyptr[1];
		return 0;
		break;
	case RM_VERIFYKEY:
		dir = va_arg(vl, char *);
		keyptr = va_arg(vl, uint64 *);
		already_decrypted = 0;
		key_holder[0] = current_dir_key[0];
		key_holder[1] = current_dir_key[1];
		current_dir_key[0] = keyptr[0];
		current_dir_key[1] = keyptr[1];
		ofd = open(dir, 0);
		if (ofd >= 0)
			close(ofd);
		already_decrypted = 0;
		current_dir_key[0] = key_holder[0];
		current_dir_key[1] = key_holder[1];
		return (ofd >= 0);
		break;
	}
	return in_hidden_dir;
}

extern void dprint(const char *sz);

static volatile uint16 *xrom = (uint16*)0x09000000;

void filesys_init(void)
{
	uint32 *ptr = (uint32 *)0x08000000;
	rootdir = NULL;

	*xrom = 0x8000;
	*xrom = 0;

	//dprint("Looking for filesys\n");
	/* Look through entire cart in 32KB increments for the magic word */
	while(ptr < (uint32*)0x0A000000)
	{
		ptr += 4*1024;
		if(*ptr == 0xFAB0BABE)
		{
			//ptr += (0xC0/4);
			strcpy(rootfile.name, "/");
			rootfile.size = 0x80000000 | ptr[1];
			rootfile.start = 0;
			rootdir = (char *)&ptr[2];
			dprint("Romfilesys found\n");
			break;
		}
	}

	memset(&rfdev, 0, sizeof(rfdev));
	rfdev.open = rf_open;
	rfdev.close = rf_close;
	rfdev.read = rf_read;
	rfdev.lseek = rf_lseek;
	rfdev.stat = rf_stat;
	rfdev.ioctl = rf_ioctl;
	device_register(&rfdev, "/rom", NULL, -1);
}

/* Skip to the next filesystem in the cart if several are present */
void filesys_next(void)
{
	uint32 *ptr = (uint32 *)((int)rootdir & 0xFFFF8000);
	ptr += 4*1024;

	while(ptr < (uint32*)0x0A000000)
	{
		ptr += 4*1024;
		if(*ptr == 0xFAB0BABE)
		{
			//ptr += (0xC0/4);
			strcpy(rootfile.name, "/");
			rootfile.size = 0x80000000 | ptr[1];
			rootfile.start = 0;
			rootdir = (char *)&ptr[2];
			dprint("Romfilesys found\n");
			break;
		}
	}
}
