#ifndef IO_H
#define IO_H

#include "core.h"
#include "device.h"

typedef void FILE;

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

FILE *fopen(const char *name, const char *mode);
int fread(void *dest, int size, int count, FILE *fp);
int fwrite(void *dest, int size, int count, FILE *fp);
int fclose(FILE *fp);
int ftell(FILE *fp);
int fseek(FILE *fp, int offset, int orgin);


#define stdin ((FILE *)0)
#define stdout ((FILE *)1)
#define stderr ((FILE *)2)

int printf(char *fmt, ...);
int _dprintf(char *fmt, ...);

int putc( int c, FILE *stream );
int putchar( int c );
int getc( FILE *stream );
int getchar( void );

int fputs( char *string, FILE *stream );
int puts( char *string );
char *gets( char *buffer );
char *fgets( char *string, int n, FILE *stream );

int fputc(int c, FILE *fp);
int fgetc(FILE *fp);

int fprintf(FILE *fp, char *fmt, ...);
int vfprintf(FILE *fp, char *fmt, va_list vl);

int rand(void);
time_t time(time_t *t);
void exit(int rc);
int atexit(void *func);
void abort(void);
void srand(unsigned int s);

typedef void DIR;

struct dirent
{
	char *d_name;
	uint32 d_size;
};

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
void closedir(DIR *dir);


#endif
