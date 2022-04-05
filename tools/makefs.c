/*
 * MAKEFS sourcecode (makefs.c) - created by Jonas Minnberg 2002
 *
 * Generates rom filesystems and adds to a GBA binary
 *
 * Part of the pogo distribution.
 * Do what you want with this but please credit me.
 * -- jonas@nightmode.org
 ****/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

unsigned char version[8] =
	{ 0x45, 0x45, 0x50, 0x52, 0x4F, 0x4D, 0x5F };

unsigned char st_orig[2][10] =
{
	{0x0E, 0x48, 0x39, 0x68, 0x01, 0x60, 0x0E, 0x48, 0x79, 0x68},
	{0x13, 0x4B, 0x18, 0x60, 0x13, 0x48, 0x01, 0x60, 0x13, 0x49}
};

unsigned char st_repl[2][10] =
{
	{0x00, 0x48, 0x00, 0x47, 0x01, 0xFF, 0xFF, 0x08, 0x79, 0x68},
	{0x01, 0x4C, 0x20, 0x47, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x08}
};

unsigned char fl_orig[2][24] =
{
	{0xD0, 0x20, 0x00, 0x05, 0x01, 0x88, 0x01, 0x22, 0x08, 0x1C, 0x10, 0x40,
	 0x02, 0x1C, 0x11, 0x04, 0x08, 0x0C, 0x00, 0x28, 0x01, 0xD0, 0x1B, 0xE0},
	{0xD0, 0x21, 0x09, 0x05, 0x01, 0x23, 0x0C, 0x4A, 0x08, 0x88, 0x18, 0x40,
	 0x00, 0x28, 0x08, 0xD1, 0x10, 0x78, 0x00, 0x28, 0xF8, 0xD0, 0x08, 0x88}
};

unsigned char fl_repl[2][24] =
{
	{0xE0, 0x20, 0x00, 0x05, 0x01, 0x88, 0x01, 0x22, 0x08, 0x1C, 0x10, 0x40,
	 0x02, 0x1C, 0x11, 0x04, 0x08, 0x0C, 0x00, 0x28, 0x01, 0xD0, 0x1B, 0xE0},
	{0xE0, 0x21, 0x09, 0x05, 0x01, 0x23, 0x0C, 0x4A, 0x08, 0x88, 0x18, 0x40,
	 0x00, 0x28, 0x08, 0xD1, 0x10, 0x78, 0x00, 0x28, 0xF8, 0xD0, 0x08, 0x88}
};

unsigned char p_repl[2][188] =
{
	{0x39, 0x68, 0x27, 0x48, 0x81, 0x42, 0x23, 0xD0, 0x89, 0x1C, 0x08,
	 0x88, 0x01, 0x28, 0x02, 0xD1, 0x24, 0x48, 0x78, 0x60, 0x33, 0xE0,
	 0x00, 0x23, 0x00, 0x22, 0x89, 0x1C, 0x10, 0xB4, 0x01, 0x24, 0x08,
	 0x68, 0x20, 0x40, 0x5B, 0x00, 0x03, 0x43, 0x89, 0x1C, 0x52, 0x1C,
	 0x06, 0x2A, 0xF7, 0xD1, 0x10, 0xBC, 0x39, 0x60, 0xDB, 0x01, 0x02,
	 0x20, 0x00, 0x02, 0x1B, 0x18, 0x0E, 0x20, 0x00, 0x06, 0x1B, 0x18,
	 0x7B, 0x60, 0x39, 0x1C, 0x08, 0x31, 0x08, 0x88, 0x09, 0x38, 0x08,
	 0x80, 0x16, 0xE0, 0x15, 0x49, 0x00, 0x23, 0x00, 0x22, 0x10, 0xB4,
	 0x01, 0x24, 0x08, 0x68, 0x20, 0x40, 0x5B, 0x00, 0x03, 0x43, 0x89,
	 0x1C, 0x52, 0x1C, 0x06, 0x2A, 0xF7, 0xD1, 0x10, 0xBC, 0xDB, 0x01,
	 0x02, 0x20, 0x00, 0x02, 0x1B, 0x18, 0x0E, 0x20, 0x00, 0x06, 0x1B,
	 0x18, 0x08, 0x3B, 0x3B, 0x60, 0x0B, 0x48, 0x39, 0x68, 0x01, 0x60,
	 0x0A, 0x48, 0x79, 0x68, 0x01, 0x60, 0x0A, 0x48, 0x39, 0x1C, 0x08,
	 0x31, 0x0A, 0x88, 0x80, 0x21, 0x09, 0x06, 0x0A, 0x43, 0x02, 0x60,
	 0x07, 0x48, 0x00, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00,
	 0x00, 0x00, 0x0E, 0x04, 0x00, 0x00, 0x0E, 0xD4, 0x00, 0x00, 0x04,
	 0xD8, 0x00, 0x00, 0x04, 0xDC, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0xFF,
	 0x08 },

	{0x22, 0x4C, 0x84, 0x42, 0x20, 0xD0, 0x80, 0x1C, 0x04, 0x88, 0x01, 
	 0x25, 0x2C, 0x40, 0x01, 0x2C, 0x02, 0xD1, 0x80, 0x1E, 0x1E, 0x49, 
	 0x2E, 0xE0, 0x00, 0x23, 0x00, 0x24, 0x80, 0x1C, 0x40, 0xB4, 0x01, 
	 0x26, 0x05, 0x68, 0x35, 0x40, 0x5B, 0x00, 0x2B, 0x43, 0x80, 0x1C, 
	 0x64, 0x1C, 0x06, 0x2C, 0xF7, 0xD1, 0x40, 0xBC, 0xDB, 0x01, 0x02, 
	 0x24, 0x24, 0x02, 0x1B, 0x19, 0x0E, 0x24, 0x24, 0x06, 0x1B, 0x19, 
	 0x19, 0x1C, 0x09, 0x3A, 0x16, 0xE0, 0x12, 0x48, 0x00, 0x23, 0x00, 
	 0x24, 0x40, 0xB4, 0x01, 0x26, 0x05, 0x68, 0x35, 0x40, 0x5B, 0x00, 
	 0x2B, 0x43, 0x80, 0x1C, 0x64, 0x1C, 0x06, 0x2C, 0xF7, 0xD1, 0x40, 
	 0xBC, 0xDB, 0x01, 0x02, 0x24, 0x24, 0x02, 0x1B, 0x19, 0x0E, 0x24, 
	 0x24, 0x06, 0x1B, 0x19, 0x08, 0x3B, 0x18, 0x1C, 0x08, 0x4C, 0x20, 
	 0x60, 0x08, 0x4C, 0x21, 0x60, 0x08, 0x49, 0x80, 0x20, 0x00, 0x06, 
	 0x02, 0x43, 0x0A, 0x60, 0x06, 0x4C, 0x20, 0x47, 0x00, 0x00, 0x00, 
	 0x0D, 0x00, 0x00, 0x00, 0x0E, 0x04, 0x00, 0x00, 0x0E, 0xD4, 0x00, 
	 0x00, 0x04, 0xD8, 0x00, 0x00, 0x04, 0xDC, 0x00, 0x00, 0x04, 0xFF, 
	 0xFF, 0xFF, 0x08 }
};

int p_size[2] = { 188, 168 };

char *find(char *data, int datasize, char *key, int keysize)
{
	char *endp = data + datasize;
	char *endk = key + keysize;
	char *keyp = key;
	int found = 0;

	while(data < endp)
	{
		if(*data == *keyp)
		{
			keyp++;
			if(keyp == endk)
				break;
		}
		else
		if(keyp != key)
		{
			keyp = key;
			data--;
		}
		data++;
	}

	return (keyp == endk) ? (data-keysize+1) : NULL;
}

char *find_freespace(char *data, int size, int freesize)
{
	int endl;
	char *endp = data + size -1;
	char fill = *endp;

	while(*endp == fill) *endp--;
	endp++;

	endl = (int)(endp-data);
	endl = (endl + 255) & 0xFFFFFF00;
	endp = &data[endl];

	if((endp + freesize) < (data + size))
		return endp;
	return NULL;
}



int patch_sram(char *rom, int size)
{
	char v0,v1,v2;
	char *v, *st, *fl, *space;
	
	v = find(&rom[180], size - 180, version, 7);

	if(v)
	{
		v0 = v[8] - '0';
		v1 = v[9] - '0';
		v2 = v[10] - '0';
		printf("Found SRAM library v%d.%d.%d at offset 0x%x\n", v0, v1, v2, v-rom);
		st = find(&rom[180], size-180, st_orig[v1-1], sizeof(st_orig[v1-1]));

		if(st)
		{
			int stloc = st - rom;

			switch(v1)
			{
			case 1:
				p_repl[v1-1][184] = stloc+0x21;
				p_repl[v1-1][186] = stloc>>16;
				break;
			case 2:
				p_repl[v1-1][164] = stloc+0x13;
				p_repl[v1-1][165] = stloc>>8;
				p_repl[v1-1][166] = stloc>>16;
				break;			 
			}

			fl = find(&rom[180], size-180, fl_orig[v1-1], sizeof(fl_orig[v1-1]));
			if(fl)
			{
				memcpy(fl, fl_repl[v1-1], sizeof(fl_repl[v1-1])); 

				space = find_freespace(rom, size, 200);
				if(space)
				{
					int spaceloc = space - rom;
					printf("Found free space at 0x%x\n", space - rom);

					switch(v1) {
					case 1:
						if (*--space == 0xFF)
						{
							p_repl[v1-1][185] = stloc >> 8;	
						} else
						{
							stloc += 0x1F;
							p_repl[v1-1][185] = stloc >> 8;
						}
						st_repl[v1-1][5] = spaceloc >> 8;
						st_repl[v1-1][6] = spaceloc >> 16;
						space++;
						break;
					case 2:
						st_repl[v1-1][7] = spaceloc >> 8;
						st_repl[v1-1][8] = spaceloc >> 16;
						break;
					}
					memcpy(st, st_repl[v1-1], sizeof(st_repl[v1-1]));
					memcpy(space, p_repl[v1-1], p_size[v1-1]); 
					printf("Patch done!\n");
				}
			}
			else
				printf("Couldn't find fl location\n");
		}
		else
			printf("Couldn't find st location\n");
	}
}

#ifdef _WIN32

#include <io.h>

struct dirent
{
	char *d_name;
};

typedef struct
{
	struct dirent de;
	struct _finddata_t fd;
	int handle;
} DIR;


DIR *opendir(char *name)
{
	char tmp[256];
	DIR *dir = malloc(sizeof(DIR));
	dir->de.d_name = NULL;
	sprintf(tmp, "%s/*", name);
	//printf("Looking for %s\n", tmp);
	dir->handle = _findfirst(tmp, &dir->fd);
	return dir;
}

struct dirent *readdir(DIR *dir)
{
	int rc = dir->handle;
	if(dir->de.d_name)
		rc = _findnext(dir->handle, &dir->fd);
	if(rc == -1)
		return NULL;
	dir->de.d_name = dir->fd.name;
	return &dir->de;
}

void closedir(DIR *dir)
{
	_findclose(dir->handle);
	free(dir);
}

#else
#include <dirent.h>
#endif // WIN32


typedef struct
{
	char name[32];
	int size;
	int start;

} RomDir;

//#define PAD_FILE(fp) { int l = ftell(fp); if(l % 4) fwrite(&l, 1, 4-(l % 4), fp); }

int fix_roms = 0;
int return_patch = 0;
int sram_patch = 0;

const unsigned char good_header[] =
{
	36,255,174,81,105,154,162,33,61,132,130,10,
	132,228,9,173,17,36,139,152,192,129,127,33,163,82,190,25,
	147,9,206,32,16,70,74,74,248,39,49,236,88,199,232,51,
	130,227,206,191,133,244,223,148,206,75,9,193,148,86,138,192,
	19,114,167,252,159,132,77,115,163,202,154,97,88,151,163,39,
	252,3,152,118,35,29,199,97,3,4,174,86,191,56,132,0,
	64,167,14,253,255,82,254,3,111,149,48,241,151,251,192,133,
	96,214,128,37,169,99,190,3,1,78,56,226,249,162,52,255,
	187,62,3,68,120,0,144,203,136,17,58,148,101,192,124,99,
	135,240,60,175,214,37,228,139,56,10,172,114,33,212,248,7
};
const unsigned char visoly[]=
{
  0x03,0x14,0xa0,0xe3,0x01,0x00,0x8f,0xe2,0x10,0xff,0x2f,0xe1,0x03,
  0xa2,0x4c,0x1c,0x1a,0x23,0x01,0xca,0x01,0xc1,0x01,0x3b,0xfb,0xd1,
  0x20,0x47,0x0f,0xa0,0x7e,0xc8,0x0d,0x80,0x30,0x1c,0x00,0xf0,0x12,
  0xf8,0x15,0x80,0x20,0x1c,0x00,0xf0,0x0e,0xf8,0x0d,0x80,0x15,0x80,
  0x0f,0xa0,0x1c,0x80,0x18,0xc8,0x1e,0x80,0x20,0x1c,0x00,0xf0,0x05,
  0xf8,0x0d,0x80,0x0d,0x49,0x0f,0x80,0x01,0xdf,0x00,0xdf,0xfa,0x27,
  0x7f,0x00,0x10,0x80,0x01,0x3f,0xfc,0xd1,0x70,0x47,0x00,0x00,0x00,
  0x00,0xa8,0xec,0x30,0x09,0x8a,0x46,0x02,0x08,0x00,0xa8,0xec,0x08,
  0x78,0x56,0x00,0x00,0x54,0x53,0x00,0x00,0x34,0x12,0x00,0x00,0xa0,
  0x68,0x02,0x08,0xcd,0xab,0x00,0x00,0x2e,0x59,0x6b,0x09,0xc0,0x00,
  0x00,0x08
};
 
/*
 If file first_child = NULL, otherwise first dir/file in directory
 next is next file/dir in parent dir
*/
typedef struct _DirFile
{
	char name[256];
	int size;
	struct _DirFile *next;
	struct _DirFile *first_child;
	int filetype;
	int offset;
} DirFile;

int bin_offset = 0;
int bincount = 0;
int smallcount = 1;
DirFile *binfiles[128];

const char spaces[] = "                                                            ";

void print_tree(DirFile *df, int l)
{
	if(df->first_child)
	{
		printf("%.*s[%s]\n", l*2, spaces, df->name);
		df = df->first_child;
		while(df)
		{
			print_tree(df, l+1);
			df = df->next;
		}
	}
	else
		printf("%.*s%s (%d bytes)\n", l*2, spaces, df->name, df->size);
}

int recsize(DirFile *df)
{
	DirFile *f;
	int size = 0;
	if(df->filetype != 1)
		size = (df->size+3)&0xfffffffc;
	if(df->first_child)
	{
		f = df->first_child;
		while(f)
		{
			size += recsize(f);
			size = (size+3)&0xfffffffc;
			f = f->next;
		}
	}
	return size;
}

int recbinsize(DirFile *df)
{
	DirFile *f;
	int size = 0;
	if(df->filetype == 1)
		size = df->size;
	if(df->first_child)
	{
		f = df->first_child;
		while(f)
		{
			size += recbinsize(f);
			f = f->next;
		}
	}
	return size;
}

void cutname(char *dst, char *src, int len)
{
	char *p = strrchr(src, '.');
	strncpy(dst, src, len);
	if(p)
		strcpy(&dst[strlen(dst)]-strlen(p), p);
}

void dump_tree(DirFile *df, FILE *outfp, int offset)
{
	RomDir dirbuf[256];
	int i,rc;
	DirFile *f;
	if(df->first_child)
	{
		/* Count files */
		f = df->first_child;
		i = 0;
		while(f)
		{
			i++;
			f = f->next;
		}
		offset += (i * sizeof(RomDir));


		/* Dump dir-file */
		f = df->first_child;
		i = 0;
		while(f)
		{
			char *p;
			memset(dirbuf[i].name, 0, 32);
			p = strrchr(f->name, '/');
			if(p)
				p++;
			else
				p = f->name;
			cutname(dirbuf[i].name, p, 31);
			//strncpy(dirbuf[i].name, p, 32);
			dirbuf[i].size = f->size;
			if(f->first_child)
				dirbuf[i].size |= 0x80000000;
			if(f->filetype == 1)
			{
				dirbuf[i++].start = bin_offset + f->offset;
				//bin_offset += recbinsize(f);
			}
			else
			{
				dirbuf[i++].start = offset;
				rc = recsize(f);
				//printf("%s has size %d\n", f->name, rc);
				offset += rc;
			}
			f = f->next;
		}
		//printf("Dumping dir-header for '%s' with %d entries\n", df->name, i);
		fwrite(dirbuf, 1, sizeof(RomDir)*i, outfp);

		f = df->first_child;
		i = 0;
		while(f)
		{
			//printf("Dumping %s to %d\n", f->name, dirbuf[i].start);
			dump_tree(f, outfp, dirbuf[i++].start);
			f = f->next;
		}
	}
	else
	if(df->size && (df->filetype != 1))
	{
		FILE *fp = fopen(df->name, "rb");
		char *ptr = malloc(df->size);
		fread(ptr, 1, df->size, fp);
		//printf("Dumping file '%s' size %d offset 0x%x \n", df->name, df->size, offset);
		fwrite(ptr, 1, (df->size+3)&0xfffffffc , outfp);
		fclose(fp);
		free(ptr);
	}
}


void make_tree(DirFile *df)
{
	DIR *dir;
	struct dirent *d;
	struct stat statbuf;
	DirFile *p, *lastp;
	int l = strlen(df->name);
	if(df->name[l-1] == '/')
		df->name[l-1] = 0;
	if(stat(df->name, &statbuf) != -1)
	{  
		if(statbuf.st_mode & S_IFDIR)
		{
			int i = 0;
			lastp = NULL;
			df->first_child = NULL;
			dir = opendir(df->name);
			p = NULL;
			while((d = readdir(dir)))
			{
				if((strlen(d->d_name) > 2) || d->d_name[0] != '.')
				{
					p = malloc(sizeof(DirFile));
					if(df->first_child == NULL)
						df->first_child = p;
					sprintf(p->name, "%s/%s", df->name, d->d_name);
					make_tree(p);
					if(lastp)
						lastp->next = p;
					lastp = p;
					i++;
				}
			}
			if(p)
				p->next = NULL;
			df->size = i*sizeof(RomDir);
			closedir(dir);
		}
		else
		{
			char *p = strrchr(df->name, '.');
			if(p)
			{
				/* Lowercase extention */
				char *q = ++p;
				while(*q)
				{
					*q = tolower(*q);
					q++;
				}

				if((strcmp(p, "bin") == 0) ||
				   (strcmp(p, "gba") == 0))
				{
					df->filetype = 1;
					binfiles[bincount++] = df;
				}
			}

			df->first_child = NULL;
			df->size = statbuf.st_size;
		}
	}
}


RomDir *rootdir = NULL;

static unsigned int magic = 0xFAB0BABE;

#define BUF_SIZE 32*1024*1024

unsigned char *buf;

int align_size = 32768;

#define ALIGN_BUF(ptr) {int l = (((int)ptr + align_size-1) & ~(align_size-1)) - (int)ptr; memset(ptr, -1, l); ptr += l;}

int main(int argc, char **argv)
{
	//int bootsize;
	int rc,i,j,l;
	//int total = 0;
	char *inrom = NULL;
	char *outrom = NULL;
	char *rootdir = NULL;
	FILE *fp, *infp;
	DirFile df;
	unsigned char *binstart;
	unsigned char *romstart;
	unsigned char *bufptr;

	buf = malloc(BUF_SIZE * 2);

	for(i=0; i<128; i++)
		binfiles[i] = NULL;

	for(i=1;i<argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'f':
				printf("Fixing ROMS\n");
				fix_roms = 1;
				break;
			case 'r':
				printf("Applying return-patch\n");
				return_patch = 1;
				break;
			case 's':
				printf("Applying sram-patch\n");
				sram_patch = 1;
				break;
			case 'x':
				printf("Using XROM alignment\n");
				align_size = 128*1024;
				break;
			}
		}
		else
		{
			if(!inrom)
				inrom = argv[i];
			else
			if(!outrom)
				outrom = argv[i];
			else
			if(!rootdir)
				rootdir = argv[i];
		}
	}


	if(!(inrom && outrom && rootdir))
	{
		printf("Usage: makefs [options] <infile> <outfile> <rootdir>\n");
		return 0;
	}

	strcpy(df.name, rootdir);
	df.next = NULL;

	infp = fopen(inrom, "rb");
	if(!infp)
	{
		printf("Could not open infile (%s)\n", inrom);
		return 0;
	}
	/*if(!fp)
	{
		printf("Could not open infile (%s)\n", outrom);
		return 0;
	}*/

	make_tree(&df);
	print_tree(&df, 0);

	ALIGN_BUF(buf);

	bufptr = buf;
	printf("Buf = %08p\n", bufptr);

	rc = fread(bufptr, 1, 256*1024, infp);
	fclose(infp);

	printf("Shell size %d bytes\n", rc);

	bufptr += rc;
	ALIGN_BUF(bufptr);
	printf("Buf = %08p\n", bufptr);

	binstart = bufptr;

	for(i=0; i<bincount; i++)
	{
		if(binfiles[i])
		{
			int readbytes = 0;

			FILE *bfp = fopen(binfiles[i]->name, "rb");


			if(!bfp)
				printf("Open failed!!!!\n");

			romstart = bufptr;

			rc = fread(bufptr, 1, 32*1024*1024, bfp);

			fclose(bfp);

			printf("%p Adding %s (%d bytes) to offset %x\n", romstart, binfiles[i]->name, rc, romstart - buf);

			if(strstr(binfiles[i]->name, ".bin") == NULL)
			{
				int cc = 0;
				if(fix_roms)
				{
					for(j=0xA0; j<0xBD; j++)
						cc += romstart[j];
					romstart[j] = (0-(0x19+cc)) & 0xff;;
					memcpy(&romstart[4], good_header, sizeof(good_header));
					romstart[0xb2] = 0x96;
				}
				
				if(sram_patch)
					patch_sram(romstart, rc);

				if(return_patch)
					memcpy(romstart, visoly, sizeof(visoly));
			}


			binfiles[i]->offset = bufptr - binstart;

			bufptr += rc;
			printf("Aligning from %p (%p)", bufptr, bufptr-buf);
			ALIGN_BUF(bufptr);
			printf("to %p (%p)\n", bufptr, bufptr-buf);

			binfiles[i]->size = bufptr - romstart;
		}
	}

	bin_offset = 0-(bufptr - binstart)-8;

	fp = fopen(outrom, "wb");

	//rc = fwrite(buf, 1, bufptr - buf, fp);
	rc = fwrite(binstart, 1, bufptr - binstart, fp);
	printf("Writing %x => %x bytes\n", bufptr - buf, rc);


#if 1
	fwrite(&magic, 1, 4, fp);
#else
	{
		int cc;
		unsigned char tmp[0xC0];

		memcpy(tmp, &magic, 4);
		memcpy(&tmp[4], good_header, 0xBC);
		memcpy(&tmp[0xA0], "POGOSHELLFILESYS", 16);

		cc = 0;
		for(j=0xA0; j<0xBD; j++)
			cc += tmp[j];
		tmp[j] = (0-(0x19+cc)) & 0xff;;

		tmp[0xb2] = 0x96;

		fwrite(tmp, 0xC0, 1, fp);
	}
#endif
	
	fwrite(&df.size, 1, 4, fp);
	dump_tree(&df, fp, 0);
	fclose(fp);
	return 0;
}
