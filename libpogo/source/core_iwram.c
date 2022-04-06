/*
 * Basic standard functions (core_misc.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Standard functions needed by the core
 *
 **/

#include "core.h"
#include "iwram.h"
#include "rtc.h"

CODE_IN_IWRAM void *memset(void *dest, int v, int l)
{	uchar *d = dest;
	while(l--)
		*d++ = v;
	return dest;
}

CODE_IN_IWRAM void *memcpy8(void *dst, const void *src, int l)
{
	uchar *d, *s;
	d = (uchar *)dst;
	s = (uchar *)src;
	while(l--)
		*d++ = *s++;
	return dst;
}

CODE_IN_IWRAM void *memcpy(void *dst, const void *src, int l)
{
	uchar *d, *s;
	uint16 *d16, *s16;
	int l16, odd;

	d = (uchar *)dst;
	s = (uchar *)src;
	odd = ((int) s & 1)^((int) d & 1);
	if (!odd) {
		if((int)s & 1)
		{
			*(d++)= *(s++);
			l--;
		}
		d16 = (uint16 *)d;
		s16 = (uint16 *)s;
		l16 = l>>1;
		while(l16--)
			*(d16++) = *(s16++);
		d = (uchar *) d16;
		s = (uchar *) s16;
		l = l&1;
	}
	while(l--)
		*(d++)= *(s++);
	return dst;
}

CODE_IN_IWRAM void *memcpy_rev(void *dst, const void *src, int l)
{
	uchar *d, *s;
	uint16 *d16, *s16;
	int l16, odd, n;

	d = (uchar *)dst + l;
	s = (uchar *)src + l;
	odd = ((int) s & 1)^((int) d & 1);
	if (!odd) {
		if((int)s & 1)
		{
			*(--d) = *(--s);
			l--;
		}
		d16 = (uint16 *)d;
		s16 = (uint16 *)s;
		l16 = l>>1;
		while(l16--)
			*(--d16) = *(--s16);
		d = (uchar *) d16;
		s = (uchar *) s16;
		l = l&1;
	}
	while(l--)
		*(--d) = *(--s);
	return dst;
}

CODE_IN_IWRAM void *memmove(void *dst, const void *src, int l)
{
	uchar *d = (uchar *)dst;
	uchar *s = (uchar *)src;

	if(d-s < l) {
		memcpy_rev(dst, src, l);
	} else
		memcpy(dst, src, l);
	return dst;
}

CODE_IN_IWRAM int strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (s1[0] - s2[0]);
}

CODE_IN_IWRAM int memcmp(const char *s1, const char *s2, int n)
{
	while(--n && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return (s1[0] - s2[0]);
}

CODE_IN_IWRAM int strncmp(const char *s1, const char *s2, int n)
{
	while(*s1 && --n && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return (s1[0] - s2[0]);
}

CODE_IN_IWRAM int strlen(const char *s)
{
	const char *t = s;
	while(*t++);
	return t-s-1;
}

CODE_IN_IWRAM char *strcpy(char *dst, const char *src)
{
	register char *d = dst;
	while(*src)
		*dst++ = *src++;
	*dst++ = 0;
	return d;
}

CODE_IN_IWRAM char *strncpy(char *dst, const char *src, int n)
{
	register char *d = dst;
	while(*src && n--)
		*dst++ = *src++;
	*dst++ = 0;
	return d;
}

