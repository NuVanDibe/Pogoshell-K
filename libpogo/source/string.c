/*
 * String functions (string.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Implements many of the standard c-library string functions.
 *
 **/

#include "core.h"

int isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

int isalpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalnum(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
		return (c+('A'-'a'));
	else
		return c;
}

int tolower(int c)
{
	if(c >= 'A' && c <= 'Z')
		return (c+('a'-'A'));
	else
		return c;
}

int atoi(const char *s)
{
	int v = 0;
	uchar neg = 0;

	while(*s == ' ')
		s++;

	if(*s == '-') {
		s++;
		neg = 1;
	}

	while(isdigit(*s)) {
		v *= 10;
		v += (*s - 0x30);
		s++;
	}
	if(neg)
		v = -v;

	return v;
}

char *itoa_size(int v, char *dst, unsigned int base, int size, char fill_char)
{
	unsigned int uv;
	static const char digits[] = "0123456789ABCDEF";
	char tmp[34];
	char *p = &tmp[33];
	uchar neg = 0;

	if (!size) size = 1;

	if(!v) {
		p = dst;
		while (size--)
			*p++ = '0';
		*p = 0;
		return dst;
	}

	*p-- = 0;
	if(v < 0 && base == 10) {
		uv = -v;
		neg = 1;
	} else
		uv = v;

	while(uv >= base) {
		*p-- = digits[uv % base];
		size--;
		uv /= base;
	}
	*p = digits[uv];
	size--;
	while(size > 0) {
		*(--p) = fill_char;
		size--;
	}

	if(neg)
		*(--p) = '-';
	strcpy(dst, p);

	return dst;
}

char *itoa(int v, char *dst, unsigned int base)
{
	return itoa_size(v, dst, base, 0, ' ');
}

int vsprintf(char *dest, char *fmt, va_list vl)
{
	int width = 0;
	int prec = 0;
	char *org = dest;
	while(*fmt) {
		switch(*fmt) {
		case '%':

			fmt++;
			if(*fmt == '%')
			{
				*dest++ = *fmt;
			}
			else
			{
				if(isdigit(*fmt) || *fmt == '.') {
					width = atoi(fmt);
					while(isdigit(*fmt))
						fmt++;
					if(*fmt == '.')
						prec = atoi(++fmt);
					while(isdigit(*fmt))
						fmt++;
				}
				if(*fmt == 'l' || *fmt == 'h')
					fmt++;
				if(!width)
					width = prec;

				switch(*fmt) {
				case 'd':
				case 'i':
				case 'u':
					itoa_size(va_arg(vl, int), dest, 10, width, '0');
					break;
				case 'p':
				case 'X':
				case 'x':
					itoa_size(va_arg(vl, int), dest, 16, width, '0');
					break;
				case 'c':
					*dest++ = (char)va_arg(vl, int);
					*dest = 0;
					break;
				case 's':
					strcpy(dest, va_arg(vl, char*));
					break;
				default:
					fmt++;
					break;
				}
				dest += strlen(dest);
				break;
			}
			break;
		default:
			*dest++ = *fmt;
		}
		fmt++;
	}
	*dest = 0;
	return org-dest;
}

int sprintf(char *dest, char *fmt, ...)
{
	int rc;
	va_list vl;
	va_start(vl, fmt);

	rc = vsprintf(dest, fmt, vl);

	va_end(vl);
	return rc;
}

char *strchr(const char *str, int c)
{
	while(*str && *str != c) str++;
	if(*str)
		return (char *)str;
	else
		return NULL;

}

char *strrchr(const char *str, int c)
{
	const char *p = NULL;
	while(*str)
	{
		if(*str == c)
			p = str;
		str++;
	}
	return (char *)p;
}

char *strcat(char *dst, const char *src)
{
	const char *s = src;
	char *d = dst;
	while(*d) d++;
	while(*s)
		*d++ = *s++;
	*d = 0;
	return dst;
}

int islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

int isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

int isprint(int c)
{
	return (c >= ' ' && c <= 127);
}

int stricmp(const char *s1, const char *s2)
{
	while(*s1 && (toupper(*s1) == toupper(*s2)))
	{
		s1++;
		s2++;
	}
	return (toupper(s1[0]) - toupper(s2[0]));
}

int strnicmp(const char *s1, const char *s2, int n)
{
	while(*s1 && --n && (toupper(*s1) == toupper(*s2)))
	{
		s1++;
		s2++;
	}

	return (toupper(s1[0]) - toupper(s2[0]));
}
