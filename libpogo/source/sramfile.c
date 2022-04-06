/*
 * SRAM Filesystem (sramfile.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * This file implements a flat sram filesystem that allows creating, writing,
 * reading, deleting etc files but no directories. It also never fragments
 * and always uses one 64KB bank.
 **/


// Use new sram format to store filedate
#ifndef OLD_VERSION
#define WITH_TIME
#endif

#ifdef WITH_TIME
#define SRAM_HEAD "MINSF3"
#else
#define SRAM_HEAD "MINSF2"
#endif

#define PACKED __attribute__ ((packed))
#include "core.h"
#include "device.h"
#include "gba_defs.h"
#include "sram_access.h"
#include "max_file_count.h"
 
/* Header of file in SRAM */
typedef struct _SRamFile
{
	struct _SRamFile *next;
	uchar name[32];
	uchar flags;
	uchar user;
	uint32 length;
#ifdef WITH_TIME
	uint32 time;
#endif
} PACKED SRamFile;

typedef struct
{
	SRamFile *file;
	uint32 pos;
	uint16 flags;
} OpenFile;

/* File has been written to */
#define OFL_WRITE 1

/* First file in SRAM */
static SRamFile *first = NULL;

/* Current 'gapfile' (The only file which has empty space after it) */
static SRamFile *gap = NULL;

//static char *sramfile_mem = (char *)0x0E000000;
//static int sram_size = 64*1024;
#ifdef WITH_TIME
uchar *sramfile_mem = (uchar *)0x0E010000;
int sram_size = 192*1024;
#else
extern char *sramfile_mem;
extern int sram_size;
#endif

/* List of open files */
static int openCount = 0;
static OpenFile openFiles[MAX_OPENFILE_COUNT];

static uchar user;

/*
void sram_setuser(int x)
{
	user = x;
}

int sram_getuser(void)
{
	return user;
}
*/

/* Set 32bit variable 'a' to 32bit value 'b' - used for SRAM variables (Because of 8bit bus) */
//#define SET32(a, b) { char *d = (char *)&(a); char *s = (char *)&(b); *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++; }

static int free_space(void);

static SRamFile *find_prev_file(const uchar *name)
{
	unsigned char fuser = 0;
	SRamFile *f, *lastf = NULL, *match = NULL;
	
	f = first;
	while(f)
	{
		/*{
			char tmp[60];
			SRamFile *xf;
			SET32(xf, f->next);
			sprintf(tmp, "FIND %p %p %p\n", f, f->name, xf);
			dprint(tmp);
		}*/

		if(user)
			sram_memcpy(&fuser, &f->user, 1);


		if(sram_strcmp(f->name, name) == 0)
		{
			if(user == fuser)
				return lastf;

			if(!(user && fuser))
				match = lastf;
		}

		lastf = f;
		SET32(f, f->next);
	}

	return match;
}


/* Create a new empty file and make it the gapfile */
static SRamFile *create_file(const uchar *name)
{
	int l;
	int zero = 0;
	SRamFile *f;

	if(free_space() < 48)
		return NULL;

	if(gap)
	{
		/* Create this file after current gapfile */
		SET32(l, gap->length);
		f = (SRamFile *)(((char *)&gap[1]) + l);
		
		SET32(f->next, gap->next);
		SET32(gap->next, f);
	}
	else
	{
		/* No gapfile means we assume this is the first file */
		first = f = (SRamFile *)&sramfile_mem[16];
		SET32(f->next, zero);
		/*{
			char tmp[70];
			SRamFile *xf;
			SET32(xf, f->next);
			sprintf(tmp, "--GAP %p %s %p %p %d\n", &f->next, name, f, xf, zero);
			dprint(tmp);
		}*/
	}
	/* New file becomes gapfile */
	gap = f;

	sram_strcpy(f->name, name);
	SET32(f->length , zero);
	sram_memcpy(&f->user, &user, 1);

#ifdef WITH_TIME
	{
		time_t t = time(NULL);
		SET32(f->time, t);
	}
#endif

	/* Save first and gap in SRAM */
	SET32(sramfile_mem[8], first);
	SET32(sramfile_mem[12], gap);

	/*{
		char tmp[50];
		SRamFile *xf;
		SET32(xf, gap->next);
		sprintf(tmp, "--GAP %s %p %p\n", name, gap, xf);
		dprint(tmp);
	}*/

	return f;
}


/* Close the gap (ie move one file close to another) between to files
   'f' is the file to move close to the file after it, 'prev' is the
   file before 'f' and 'up' tells wether we should move the file
   following 'f' upwards towards 'f' - or move 'f' downwards towards
   the file that follows it.
*/
static SRamFile *close_gap(SRamFile *f, SRamFile *prev, int up)
{
	int l;
	uchar *dest;
	SRamFile *next;
		
	SET32(l, f->length);
	SET32(next, f->next);

	if(up)
	{
		/* Move next file up to directly after this file */
		dest = (((uchar *)&f[1]) + l); /* Ptr to after file */
		SET32(l, next->length);

		sram_memcpy(dest, (uchar *)next, l + sizeof(SRamFile));
		SET32(f->next, dest);
	}
	else
	{
		/* Move this file down to directly before next */
		if(next)
			dest = (uchar *)next - l - sizeof(SRamFile);
		else
			dest = ((uchar *)&sramfile_mem[sram_size]) - l - sizeof(SRamFile);
		sram_memmove(dest, (uchar *)f, l + sizeof(SRamFile));

		SET32(prev->next, dest); 
		f = (SRamFile*)dest;
	}
	return f;
}

/* Make a file current gapfile by moving all files under it downwards or
   all files above it (including file) upwards */
static SRamFile *bump_file(SRamFile *bfile, int size)
{
	SRamFile *next = NULL, *f;
	SRamFile *stack[MAX_FILE_COUNT];
	int sp = 0;

	if(gap < bfile)
	{
		SRamFile *nf = NULL;
		/* Gap is before file, move files after gap (including file) upwards */
		f = gap;
		while(next != bfile)
		{

			SET32(next, f->next);
			nf = close_gap(f, NULL, 1);
			SET32(f, f->next);
		}
		SET32(bfile, nf->next);
	}
	else
	{ 
		/* Gap is after file, move files before gap (including gap) downwards */
		f = bfile;
		while(f != gap)
		{
			stack[sp++] = f;
			SET32(f, f->next);
		}
		stack[sp] = gap;

		while(sp)
		{
			close_gap(stack[sp], stack[sp-1], 0);
			sp--;
		}
	}

	gap = bfile;

	/* Save gap in SRAM */
	SET32(sramfile_mem[12], gap);
	return gap;
}

static int free_space(void)
{
	SRamFile *next;
	int l;

	if(!gap)
		return sram_size;

	SET32(next, gap->next);
	SET32(l, gap->length);

	if(!next)
		next = (SRamFile *)&sramfile_mem[sram_size];

	return ((int)next - (int)gap) - sizeof(SRamFile) - l;
}

/*

extern int _dprintf(char *fmt, ...);
static void print_sys(void)
{
	int l, g;
	SRamFile *f, *next;

	f = first;

	while(f)
	{
		SET32(l, f->length);
		SET32(next, f->next);
		if(!next)
			g = ((uchar *)&sramfile_mem[sram_size]) - (((uchar *)&f[1]) + l);
		else
			g = ((uchar *)next) - (((uchar *)&f[1]) + l);
		//_dprintf(2, "File %s at %p (%d bytes) with %d bytes gap\n", f->name, f, l, g);
		f = next;
	}
}
*/

static int sram_stat(const uchar *name, struct stat *buffer)
{
	int l;
	SRamFile *f, *lastf;
	SET32(f, first->next);

	while(*name == '/')
		name++;
	if(!*name)
	{
		buffer->st_size = 0;
		buffer->st_mode = S_IFDIR;
		return 0;
	}

	lastf = find_prev_file(name);

	if(lastf)
	{
		uchar usr;
		SET32(f, lastf->next);
		sram_memcpy(&usr, &f->user, 1);
		SET32(l, f->length);
		buffer->st_size = l;
		buffer->st_mode = 0;
		buffer->st_uid = usr;
#ifdef WITH_TIME
		{
			time_t t;
			SET32(t, f->time);
			buffer->st_atime = buffer->st_ctime = t;
		}
#endif
		return 0;
	}

	return -1;
}

/* Open a file (creating a new file if necessary */
static int sram_open(const char *name, int mode)
{
	SRamFile *lastf = NULL;
	SRamFile *f;
	
	SET32(f, first->next);
	while(*name == '/')
		name++;

	if(!*name)
	{
		/* Empty name means read filelist */
		OpenFile *of = &openFiles[openCount++];
		of->file = 0;
		of->pos = (uint32)f;
		of->flags = 0;
		return openCount-1;
	}

	lastf = find_prev_file(name);

	if(lastf)
	{
		SET32(f, lastf->next);
		OpenFile *of = &openFiles[openCount++];
		of->file = f;
		of->pos = 0;
		of->flags = 0;
		return openCount-1;
	}

	if(mode & O_CREAT)
	{
		if((f = create_file(name)))
		{
			OpenFile *of = &openFiles[openCount++];
			of->file = f;
			of->pos = 0;
			return openCount-1;
		}
	}

	return -1;
}

static int sram_write(int fd, const void *src, int size)
{
	uchar *p;
	int l;

	OpenFile *of = &openFiles[fd];
	SRamFile *f = of->file;

	SET32(l, f->length);

	if(of->pos == -1) {
		/* Writing to closed file */
		return -1;
	}

	if(((int)of->pos + size + 8)-l > free_space())
	{
		//_dprintf("SRAM FULL! %d %d %d %d", of->pos, size, l, free_space());
		return -1;
	}
	if((of->pos + size > l) && gap != f)
		of->file = f = bump_file(f, size);

	p = ((uchar *)&f[1]) + of->pos;

	of->flags |= OFL_WRITE;

#ifdef WITH_TIME
	{
		time_t t = time(NULL);
		SET32(f->time, t);
	}
#endif
	sram_memcpy(p, src, size);
	of->pos += size;
	if(of->pos > l)
		SET32(f->length, of->pos);

	return size;
}

static int sram_read(int fd, void *dest, int size)
{
	uchar *p;
	uint32 l;
	OpenFile *of = &openFiles[fd];
	SRamFile *f = of->file;

	if (f) {
		//fprintf(stderr, "sram_read(%d, %p, %d);\n", fd, dest, size);
		p = ((uchar *)&f[1]) + of->pos;
		SET32(l, f->length);
	} else {
		//fprintf(stderr, "sram_read(%d, %p, %d); //raw\n", fd, dest, size);
		p = (uchar *) of->pos;
		l = (unsigned int) sramfile_mem + sram_size;
	}


	if (of->pos == 0xFFFFFFFF) {
		/* Reading from closed file */
		return -1;
	}

	of->pos += size;
	if(of->pos > l)
	{
		size -= (of->pos - l);
		of->pos = l;
	}

	sram_memcpy(dest, p, size);

	return size;
}

static int sram_readdir_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
	int fd = (int) dir;
	unsigned char fuser = 0;
	OpenFile *of = &openFiles[fd];
	SRamFile *f = of->file;

	if(f) {
		*result = NULL;
		return EBADF;
	}

	while (of->pos)
	{
		f = (SRamFile *)of->pos;

		if(user)
			sram_memcpy(&fuser, &f->user, 1);

		if(!fuser || (user == fuser))
		{
			sram_memcpy(entry->d_name, f->name, 32);
			sram_memcpy(&entry->d_size, &f->length, 4);
			SET32(f, f->next);
			of->pos = (int32)f;
			*result = entry;
			return 0;
		}

		SET32(f, f->next);
		of->pos = (int32)f;
	}

	*result = NULL;
	return 0;
}

static int sram_lseek(int fd, int offset, int orgin)
{
	char *p;
	int l;
	int newpos = 0;
	OpenFile *of = &openFiles[fd];
	SRamFile *f = of->file;

	p = ((char *)&f[1]) + of->pos;

	SET32(l, f->length);

	if(of->pos == -1) {
		/* Reading from closed file */
		return -1;
	}

	switch(orgin)
	{
	case SEEK_SET:
		newpos = offset;
		break;
	case SEEK_CUR:
		newpos = of->pos+offset;
		break;
	case SEEK_END:
		newpos = l+offset;
		break;
	case SEEK_MEM:
		return 0;
	}

	if(newpos > l)
		newpos = l;
	if(newpos < 0)
		newpos = 0;
	of->pos = newpos;

	return newpos;
}

static int sram_close(int fd)
{
	uint32 p;
	OpenFile *of = &openFiles[fd];
	SRamFile *f = of->file;

	if(of->flags & OFL_WRITE)
	{
		SET32(p, of->pos);

		/* File has been written to - must become gap */
		if(f != gap)
			f = bump_file(f, p);

		/* Truncate file to current pos */
		SET32(f->length, p);
	}

	of->pos = -1;

	of = &openFiles[openCount-1];
	while(openCount && of->pos == -1) {
		openCount--;
		of = &openFiles[openCount-1];
	}
	return 0;
}

static int sram_remove(const char *name)
{
	uint32 l;
	SRamFile *lastf, *f, *sf;

	lastf = find_prev_file(name);

	if(lastf)
	{
		SET32(f, lastf->next);
		SET32(l, lastf->length);
		lastf = bump_file(lastf, l);
		SET32(sf, f->next);
		SET32(lastf->next, sf);

		return 0;
	}

	return -1;
}



/* Remove a file */
/*
static int sram_remove(const char *name)
{
	uint32 l;
	unsigned char fuser = 0;
	SRamFile *lastf = NULL, *sf, *f = first;
	while(f)
	{
		if(user)
			sram_memcpy(&fuser, &f->user, 1);

		if((user == fuser) && sram_strcmp(f->name, name) == 0)
		{
			if(!lastf)
				return -1; // We currently dont support removing first file

			// Bump the last file so the removing this file only leaves a single gap
			SET32(l, lastf->length);
			lastf = bump_file(lastf, l);

			SET32(sf, f->next);
			SET32(lastf->next, sf);
			return 0;
		}
		lastf = f;
		SET32(f, f->next);
	}
	return -1;
}
*/

static int sram_ioctl(int fd, int req, va_list vl)
{
	switch(req)
	{
	case SR_GETUSER:
		return user;
		break;
	case SR_SETUSER:
		user = va_arg(vl, int);
		return 0;
		break;
	}
	return free_space();
}


static Device sramdev;

static int init = 0;
extern int start_keys;

#ifdef WITH_TIME
void sram_init(void)
#else
void sram_init_old(void)
#endif
{
	int zero = 0;
	user = 0;

	//fprintf(2, "start %p, size %d\n", sramfile_mem, sram_size);

	if(!init)
	{
		memset(&sramdev, 0, sizeof(Device));
		sramdev.open = sram_open;
		sramdev.close = sram_close;
		sramdev.read = sram_read;
		sramdev.write = sram_write;
		sramdev.remove = sram_remove;
		sramdev.ioctl = sram_ioctl;
		sramdev.lseek = sram_lseek;
		sramdev.stat = sram_stat;
		sramdev.readdir_r = sram_readdir_r;
		device_register(&sramdev, "/sram", NULL, -1);
	}

	if((start_keys != (U_KEY|L_BUTTON)) && (sram_strcmp(sramfile_mem, (uchar *)SRAM_HEAD) == 0))
	{
		SET32(first, sramfile_mem[8]);
		SET32(gap, sramfile_mem[12]);
	}
	else
	{
		first = gap = NULL;
		sram_memcpy(sramfile_mem, (uchar *) SRAM_HEAD, 8);
		SET32(sramfile_mem[8], zero);
		SET32(sramfile_mem[12], zero);

		// Make sure we have an empty first file
		create_file("DUMMY");
		//dprint("Clearing SRAM\n");
	}

	init = 1;
}

//static unsigned int magic[3] =
//{
//	0xDE51CE01, (unsigned int)sram_init, (unsigned int)magic,
//};
