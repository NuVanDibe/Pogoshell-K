#ifndef STRING_H
#define STRING_H

#include "core.h"

void *memcpy(void *dst, const void *src, int l);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
int strlen(const char *s);
char *strcat(char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
char *itoa(int v, char *dst, unsigned int base);

int vsprintf(char *dest, char *fmt, va_list vl);
int sprintf(char *dest, char *fmt, ...);

int toupper(int c);
int tolower(int c);

int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, int n);

int atoi(const char *str);

int isdigit(int c);
int isprint(int c);
int islower(int c);
int isalnum(char c);
int isalpha(char c);

int isspace(char c);



#endif
