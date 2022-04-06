#ifndef CORE_H
#define CORE_H

//#define MALLOC_DEBUG
#include "iwram.h"

typedef unsigned long long uint64;
typedef long long int64;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uchar;

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF (-1)
#endif

// Map stdargs to gcclibs implementation - should be in its own file
typedef __builtin_va_list va_list; 
#define va_start(v,l) __builtin_stdarg_start((v),l)
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg


void *memset(void *dest, int v, int l) CODE_IN_IWRAM;
void *memmove(void *dst, const void *src, int l) CODE_IN_IWRAM;
void *memmove8(void *dst, const void *src, int l) CODE_IN_IWRAM;
void *memcpy(void *dst, const void *src, int l) CODE_IN_IWRAM;
void *memcpy8(void *dst, const void *src, int l) CODE_IN_IWRAM;
int strcmp(const char *s1, const char *s2) CODE_IN_IWRAM;
int strncmp(const char *s1, const char *s2, int n) CODE_IN_IWRAM;
int strlen(const char *s) CODE_IN_IWRAM;
char *strcpy(char *dst, const char *src) CODE_IN_IWRAM;
char *strncpy(char *dst, const char *src, int n) CODE_IN_IWRAM;
int memcmp(const char *s1, const char *s2, int n) CODE_IN_IWRAM;

void memory_init(uint32 *ptr, int size);
void *memory_alloc(int size);
void memory_free(void *mem);
int memory_avail(void);

void Halt(void);
void set_ram_start(int i);

typedef uint32 time_t;
#define CLOCKS_PER_SEC 50
int clock(void);

struct tm
{
	uint16 tm_sec;     /* seconds */
	uint16 tm_min;     /* minutes */
	uint16 tm_hour;    /* hours */
	uint16 tm_mday;    /* day of the month */
	uint16 tm_mon;     /* month */
	uint16 tm_year;    /* year */
	uint16 tm_wday;    /* day of the week */
	//int     tm_yday;        /* day in the year */
	//int     tm_isdst;       /* daylight saving time */
};

time_t time(time_t *);
struct tm *time2(struct tm *dst);
struct tm *localtime(time_t t);

#define gmtime localtime



#define memmove8 memmove

#define CLOCKS_PER_SEC 50

#ifdef MALLOC_DEBUG

void *mdbgptr;

#define malloc(x) (_dprintf("%s:%d %d bytes => %p\n", __FILE__, __LINE__, x, mdbgptr = memory_alloc(((x)+3)/4) ), mdbgptr )
#define realloc(p, l)  (_dprintf("%s:%d %d bytes => %p\n", __FILE__, __LINE__, x, mdbgptr = memory_realloc(p,((l)+3)/4) ), mdbgptr )
#define free(p) memory_free(p)

#else

void *malloc(int x);
void *realloc(void *p, int l);
void free(void *x);

#endif

#endif
