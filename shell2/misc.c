#include <pogo.h>
#include "misc.h"

extern void LZ77UnCompWram(void *Srcp, void *Destp);

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

uchar *file2mem(char *fname, void *mem, int msize)
{
	char *s;
	int fsize;
	int fd = open(fname, 0);
	uchar *ptr;

	s = fname;
	while(*s++ == '.');
	while(*s++ != '.');

	fsize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	ptr = (uchar *)lseek(fd, 0, SEEK_MEM);
	if(mem && !ptr)
	{
		// Read file to top of buffer
		if(s[2] == 'z')
			ptr = mem + msize - fsize;
		else
			ptr = mem;

		read(fd, ptr, fsize);
	}
	close(fd);

	//fprintf(stderr, "FILE %s\n", s);

	if(mem && s[2] == 'z')
	{
		LZ77UnCompWram(ptr, mem);
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
