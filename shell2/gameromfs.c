
#include <pogo.h>

static char *games[MAX_OPENFILE_COUNT];
static int game_count = 0;

typedef struct {
	int pos;
	int rom;
} OpenFile;

static int open_files = 0;
static OpenFile open_filetab[MAX_OPENFILE_COUNT];


static Device gsdev;

static int find_file(const char *name)
{
	char *s;
	char tmp[128];
	int i;

	while(*name == '/')
		name++;

	if(!strlen(name))
		return -1;

	strcpy(tmp, name);
	s = strrchr(tmp, '.');
	if(strcmp(s, ".gba") == 0)
		*s = 0;

	for(i=0; i<game_count; i++)
	{
		if(strnicmp(&games[i][0xa0], tmp, 12) == 0)
			return i;
	}

	return -2;
}

static int gs_open(const char *name, int flags)
{
	int i;
	OpenFile *f;

	i = find_file(name);
	
	if(i >= -1)
	{
		f = &open_filetab[open_files++];
		f->rom = i;
		f->pos = 0;
		return open_files-1;
	}

	return -1;
}


static int gs_read(int fd, void *dest, int size)
{
	uchar *p;
	OpenFile *f;

	//fprintf(stderr, "READ %d bytes\n", size);

	f = &open_filetab[fd];

	if(f->rom < 0)
	{
		p = (uchar *) (0x08000000 + f->pos);
		f->pos += size;

		if (f->pos > 0x09FFFFFF) {
			size -= (0x09FFFFFF - f->pos);
			f->pos = 0x09FFFFFF;
		}

		memcpy(dest, p, size);

		return size;
	}

	memcpy(dest, &games[f->rom][f->pos], size);
	f->pos += size;

	return size;
}

static int gs_readdir_r(DIR *dir, struct dirent *entry, struct dirent **result)
{
	int fd = (int) dir;
	OpenFile *f;
	int i, gn;
	char *ptr;

	f = &open_filetab[fd];

	if(f->rom >= 0) {
		*result = NULL;
		return EBADF;
	}

	if (f->pos % sizeof(Romfile)) {
		*result = NULL;
		return ENOENT;
	}

	gn = f->pos / sizeof(Romfile);

	if (gn >= game_count) {
		*result = NULL;
		return 0;
	}

	ptr = &games[gn][0xa0];

	entry->d_name[0] = toupper(ptr[0]);
	for(i=1; i<12; i++)
		entry->d_name[i] = tolower(ptr[i]);

	entry->d_name[12] = 0;
	strcat(entry->d_name, ".gba");
	entry->d_size = games[gn+1] - games[gn];
	if (entry->d_size > 32*1024*1024)
		entry->d_size = 32*1024*1024;

	f->pos += sizeof(Romfile);

	*result = entry;
	return 0;
}

static int gs_close(int fd)
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

static int gs_stat(const char *name, struct stat *stat)
{
	int i;

	memset(stat, 0, sizeof(struct stat));

	i = find_file(name);

	if(i >= -1)
	{
		if(i >= 0)
			stat->st_size = games[i+1] - games[i];
		else
			stat->st_mode = S_IFDIR;

		if(stat->st_size > 32*1024*1024) stat->st_size = 32*1024*1024;


		return 0;
	}

	return -1;
}


static int gs_lseek(int fd, int offset, int origin)
{
	int size, newpos = 0;
	OpenFile *f;

	f = &open_filetab[fd];

	size = games[f->rom+1] - games[f->rom];
	if(size > 32*1024*1024) size = 32*1024*1024;

	switch(origin) {
	case SEEK_SET:
		newpos = offset;
		break;
	case SEEK_CUR:
		newpos = f->pos + offset;
		break;
	case SEEK_END:
		newpos = size - offset;
		break;
	case SEEK_MEM:
		//fprintf(stderr, "memseek %p\n", &games[f->rom][f->pos]);
		return (int)&games[f->rom][f->pos];
		break;
	}

	if(newpos < 0)
		return -1;
	if(newpos > size) {
		newpos = size;
		//dprint("Seeking beyond end of file\n");
	}

	return f->pos = newpos;
}

extern void dprint(const char *sz);

void gamesys_init(void)
{
	int i;
	int cart_size = 32*1024*1024;
	char *filestart = NULL;
	char *ptr = (char *)0x08000000;

	while(ptr[0xbd] == ptr[cart_size/2 + 0xbd] &&
	      ptr[0xbd + 0x8000] == ptr[cart_size/2 + 0xbd + 0x8000] &&
	      ptr[0xbd + 0x10000] == ptr[cart_size/2 + 0xbd + 0x10000] &&
	      ptr[0xbd + 0x18000] == ptr[cart_size/2 + 0xbd + 0x18000])
	{
		cart_size /= 2;
	}

	fprintf(stderr, "Cart size = %x\n", cart_size);

	ptr += 0x8000;

	/* Look through entire cart in 32KB increments for roms */
	while(ptr < (char*)(cart_size + 0x08000000))
	{
		if( ((ptr[3] & 0xFE) == 0xEA) && (ptr[0xb2] == 0x96))
		{
			char *p = &ptr[0xa0];

			if(*p)
			{
				char tmp[14];
				strcpy(tmp, p);
				tmp[12] = 0;
				//fprintf(stderr, "Found ROM %s at %p\n", tmp, ptr);

				for(i=0; i<12; i++)
				{
					if(p[i] < 0x20 || p[i] > 0x7F)
						break;
				}

				if(i == 12 || !p[i])
					games[game_count++] = ptr;
					// Found a ROM
			}
		}

		if(*((uint32 *)ptr) == 0xFAB0BABE)
			filestart = ptr;

		ptr += 32*1024;
	}

	if(filestart && (filestart > games[game_count-1]))
		games[game_count] = filestart;
	else
		games[game_count] = games[game_count-1] + 8*1024*1024;

	memset(&gsdev, 0, sizeof(gsdev));
	gsdev.open = gs_open;
	gsdev.close = gs_close;
	gsdev.read = gs_read;
	gsdev.lseek = gs_lseek;
	gsdev.stat = gs_stat;
	gsdev.readdir_r = gs_readdir_r;
	device_register(&gsdev, "/cartroms", NULL, -1);
}
