#include <pogo.h>
#include "gba_defs.h"
#include "misc.h"
#include "unapack.h"

extern void LZ77UnCompWram(void *Srcp, void *Destp);

#if 0
#define ROM_BANKSWITCH (volatile u16 *)(0x096B592E)
#define WRITE_LOC_1 (volatile u16 *)(0x987654*2+0x8000000)
#define WRITE_LOC_2 (volatile u16 *)(0x012345*2+0x8000000)
#define WRITE_LOC_3 (volatile u16 *)(0x007654*2+0x8000000)
#define WRITE_LOC_4 (volatile u16 *)(0x765400*2+0x8000000)
#define WRITE_LOC_5 (volatile u16 *)(0x013450*2+0x8000000)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

static void *pmem_ptr = (void *) (0x02000000);
static int pmem_free = 256*1024;

#if 0
void reset_gba(void)
{
  unsigned int i;

  SETW(REG_IE, 0);
  SETW(REG_IF, 0);
  SETW(REG_IME, 0);
  SETW(REG_SOUNDBIAS, 0x0200);

  /* reset cart bankswitching */
  for(i=0;i<1;i++) *WRITE_LOC_1=0x5354;
  for(i=0;i<500;i++) *WRITE_LOC_2=0x1234;
  for(i=0;i<1;i++) *WRITE_LOC_2=0x5354;
  for(i=0;i<500;i++) *WRITE_LOC_2=0x5678;
  for(i=0;i<1;i++) *WRITE_LOC_1=0x5354;
  for(i=0;i<1;i++) *WRITE_LOC_2=0x5354;
  for(i=0;i<1;i++) *WRITE_LOC_4=0x5678;
  for(i=0;i<1;i++) *WRITE_LOC_5=0x1234;
  for(i=0;i<500;i++) *WRITE_LOC_2=0xabcd;
  for(i=0;i<1;i++) *WRITE_LOC_1=0x5354;
  *ROM_BANKSWITCH=0;

  /* reset GBA */
  *(u16 *)0x03007ffa = 0;  /* reset to ROM (= 0) rather than RAM (= 1) */
  *((u32*)0x0203fc08) = 0; /* signal plugin exit to Pogoshell */
  asm volatile(
    "mov r0, #0xfc  \n"  /* clear everything other than RAM */
    "swi 0x01   \n"
    "swi 0x00   \n"
    ::: "r0", "r1", "r2", "r3");
}
#endif

int gethex(char *p)
{
	int l = 0;

	while((*p >= '0' && *p <= '9') ||
		  (*p >= 'A' && *p <= 'F') ||
		  (*p >= 'a' && *p <= 'f'))
	{
		if(*p <= '9')
			l = (l << 4) | ((*p++) - '0');
		else if (*p <= 'F')
			l = (l << 4) | ((*p++) - 'A' + 10);
		else
			l = (l << 4) | ((*p++) - 'a' + 10);
	}
		
	return l;
}

char *strdup(char *str)
{
	char *p = malloc(strlen(str)+1);
	strcpy(p, str);
	return p;
}

char *basename(char *str)
{
	char *p = strrchr(str, '/');
	if(p)
		return p+1;
	else
		return str;
}

void *pmalloc(int size)
{
	void *ret;
	if (size > pmem_free)
		return NULL;

	ret = pmem_ptr;
	pmem_ptr += size;
	pmem_free -= size;

	return ret;
}

int pmemory_free(void)
{
	return pmem_free;
}

void *pmemory_pointer(void)
{
	return pmem_ptr;
}

void pfree(void)
{
	pmem_ptr = (void *) (0x02000000);
	pmem_free = 256*1024;
}

int read_line(char *line, int size, FILE *fp)
{
	char *p;
	if(fgets(line, size, fp))
	{
		p = &line[strlen(line)-1];
		while(*p == 10 || *p == 13 || *p == 32)
			p--;
		p[1] = 0;

		//fprintf(2, "got \"%s\"\n", line);

		if(*line == '#' || !strlen(line))
			return 0;

		if(*line == '[' && *p == ']')
			return -1;

		return 1;
	}
	return -2;
}


int find_section(FILE *fp, char *name)
{
	char tmp[24];
	char line[32];
	sprintf(tmp, "[%s]", name);

	fseek(fp, 0, SEEK_SET);

	//fprintf(2, "Looking for %s\n", tmp);

	while(read_line(line, sizeof(line), fp) != -2)
	{

		if(strcmp(line, tmp) == 0)
		{
			//fprintf(2, "'%s' == '%s'\n", tmp, line);
			return 1;
		}
	}
	//fprintf(2, "Didnt find %s\n", name);
	return 0;
}

// Unused
#if 0
int file2ram(char *fname, void *mem, int msize)
{
	char *s;
	int fsize;
	int fd = open(fname, 0);
    unsigned int header;
	uchar *ptr;

	s = fname;
	while(*s++ == '.');
	while(*s++ != '.');

	fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

    // Don't want to overflow
    if (fsize > msize)
       return -1;
	
    // Read file to top of buffer
	if(s[2] == 'z')
		ptr = mem + msize - fsize;
	else
		ptr = mem;

	read(fd, ptr, fsize);
	close(fd);

	//fprintf(stderr, "FILE %s\n", s);

	if(mem && s[2] == 'z')
	{
        header = ((unsigned int *) ptr)[0];
        fsize = (header>>8);
        // Check that type is lz77 and size fits memory
        if (((header>>4)&7) != 1 && fsize > msize)
           return -1;
		LZ77UnCompWram(ptr, mem);
		ptr = mem;
	}

	return fsize;
}
#endif

char *file2mem(char *fname, void *mem, int msize, int decompress)
{
	char *s;
	int fsize;
	int fd = open(fname, 0);
	char *ptr;

	s = fname;
	while(*s++ == '.');
	while(*s++ != '.');

	fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	ptr = (char *)lseek(fd, 0, SEEK_MEM);
	close(fd);

	if ((!mem && decompress) || !ptr)
		return NULL;

	if (mem)
	{
		switch (decompress)
		{
			case LZ77:
				LZ77UnCompWram(ptr, mem);
				break;
			case APACK:
				depack(ptr, mem);
				break;
			case RAW:
			default:
				if (fsize > msize)
					return NULL;
				memcpy(mem, ptr, fsize);
				break;
		}
		ptr = mem;
	}

	return ptr;
}


int parse_assignment(char *line, char **name, char **val)
{
	char *p;
	if(strchr(line, '='))
	{
		p = line;
		while(!isalpha(*p)) p++;
		*name = p;
		while(*p != '=') p++;
		*p++ = 0;
		*val = p;
		while(*p >= 0x20) p++;
		*p = 0;
		return 1;
	}
	return 0;
}

Font *font_load_path(char *name)
{
	Font *f;
	char tmp[80];
	strcpy(tmp, GET_PATH(FONTS));
	strcat(tmp, name);
	//sprintf(tmp, ".shell/fonts/%s", name);
	f = font_load(tmp);
	//fprintf(stderr, "%s -> %p\n", tmp, f);
	return f;
}
