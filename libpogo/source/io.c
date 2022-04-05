/*
 * IO functions (io.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Implements many of the standard c-library IO functions.
 *
 **/

#include "core.h"
#include "string.h"
#include "device.h"
#include "io.h"
#include "gba_defs.h"

int ftell(FILE *fp)
{
	return tell((int)fp);
}

int fseek(FILE *fp, int offset, int orgin)
{
	return lseek((int)fp, offset, orgin);
}

FILE *fopen(const char *name, const char *mode)
{
	int fd;
	int m = 0;
	const char *p = mode;
	while(*p) {
		if(*p == 'w')
			m |= O_CREAT;
		p++;
	}
	
	fd = open(name, m);

	if(fd != -1)
		return (FILE *)fd;
	else
		return NULL;
}

int fread(void *dest, int size, int count, FILE *fp)
{
	return read((int)fp, dest, size*count);
}


int fwrite(void *dest, int size, int count, FILE *fp)
{
	return write((int)fp, dest, size*count);
}

int fclose(FILE *fp)
{
	return close((int)fp);
}

int fflush(FILE *fp)
{
	return 0;
}

int printf(char *fmt, ...)
{
	char tmp[256];
	int rc;
	va_list vl;
	va_start(vl, fmt);

	rc = vsprintf(tmp, fmt, vl);
	write(FD_STDOUT, tmp, strlen(tmp));

	va_end(vl);
	return rc;
}

void dprint(const char *sz);

int _dprintf(char *fmt, ...)
{
	char tmp[256];
	int rc;
	va_list vl;
	va_start(vl, fmt);

	rc = vsprintf(tmp, fmt, vl);
	//write(FD_STDERR, tmp, strlen(tmp));
	dprint(tmp);
	va_end(vl);
	return rc;
}

int fprintf(FILE *fp, char *fmt, ...)
{
	char tmp[256];
	int rc;
	va_list vl;
	va_start(vl, fmt);

	rc = vsprintf(tmp, fmt, vl);
	write((int)fp, tmp, strlen(tmp));

	va_end(vl);
	return rc;
}

int vfprintf(FILE *fp, char *fmt, va_list vl)
{
	char tmp[256];
	int rc;

	rc = vsprintf(tmp, fmt, vl);
	write((int)fp, tmp, strlen(tmp));
	return rc;
}

int putc(int c, FILE *fp)
{
	unsigned char cc = c;
	write((int)fp, &cc, 1);
	return c;
}


int putchar(int c)
{
	unsigned char cc = c;
	write(FD_STDOUT, &cc, 1);
	return c;
}

int getc(FILE *fp)
{
	unsigned char c;
	if(read((int)fp, &c, 1) < 1)
		return EOF;
	else
		return c;
}

int fputc(int c, FILE *fp)
{
	return putc(c, fp);
}

int fgetc(FILE *fp)
{
	return getc(fp);
}

int getchar(void)
{
	unsigned char c;
	if(read(FD_STDIN, &c, 1) < 1)
		return EOF;
	else
		return c;
}

int fputs(char *string, FILE *fp)
{
	return write((int)fp, string, strlen(string));
}

int puts(char *string )
{
	char lf = 10;
	write(FD_STDOUT, string, strlen(string));
	return write(FD_STDOUT, &lf, 1);
}

char *fgets(char *buffer, int n, FILE *fp)
{
	int c = 0;
	char *ptr = buffer;
	n--;
	while((ptr-buffer < n) && c != 10)
	{
		if(read((int)fp, ptr, 1) == 1)
			c = *ptr++;
		else
			c = 10;	
	}
	*ptr = 0;
	if(!strlen(buffer))
		return NULL;

	return buffer;
}

char *gets(char *buffer)
{
	int c = 0;
	char *ptr = buffer;

	while(c != 10)
	{
		while((c = getchar()) == EOF);
		if(c == 8)
		{
			if(ptr > buffer)
				ptr--;
		}
		else
		if(c != 10)
			*ptr++ = c;
	}
	*ptr = 0;
	return buffer;
}



DIR *opendir(const char *name)
{
	struct stat s;
/*	char tmp_name[256];
	int i;

	i = strlen(name);

	strcpy(tmp_name, name);
	if (tmp_name[i-1] == '/')
		tmp_name[i-1] = '\0'; */
	if((stat(/*tmp_name*/name, &s) >= 0) && (s.st_mode & S_IFDIR))
	{
		return fopen(/*tmp_name*/name, "rb");
	}
	return NULL;
}


static struct dirent de;
static Romfile rf;
static char name[34];

struct dirent *readdir(DIR *dir)
{
	int rc;
	if((rc = fread(&rf, 1, sizeof(rf), dir)) == sizeof(rf))
	{
		strncpy(name, rf.name, 32);
		de.d_name = name;
		de.d_size = rf.size;
		return &de;
	}
	return NULL;
}

void closedir(DIR *dir)
{
	fclose(dir);
}

