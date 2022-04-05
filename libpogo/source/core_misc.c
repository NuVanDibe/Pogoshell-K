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
#include "rtc.h"

void *memset(void *dest, int v, int l)
{	uchar *d = dest;
	while(l--)
		*d++ = v;
	return dest;
}

void *memcpy8(void *dst, const void *src, int l)
{
	uchar *d, *s;
	d = (uchar *)dst;
	s = (uchar *)src;
	while(l--)
		*d++ = *s++;
	return dst;
}

void *memcpy(void *dst, const void *src, int l)
{
	uchar *d, *s;
	uint16 *d16, *s16;

	if( ((int)src & 1) || ((int)dst & 1) || (l & 1))
	{
		d = (uchar *)dst;
		s = (uchar *)src;
		while(l--)
			*d++ = *s++;
	} else {
		d16 = (uint16 *)dst;
		s16 = (uint16 *)src;
		l /= 2;
		while(l--)
			*d16++ = *s16++;
	}
	return dst;
}

void *memmove(void *dst, const void *src, int l)
{
	uchar *d = (uchar *)dst;
	uchar *s = (uchar *)src;

	if(d-s < l) {
		d+=l;
		s+=l;
		while(l--)
			*(--d)= *(--s);
	} else
		while(l--)
			*d++ = *s++;
	return dst;
}

int strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (s1[0] - s2[0]);
}

int memcmp(const char *s1, const char *s2, int n)
{
	while(--n && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return (s1[0] - s2[0]);
}

int strncmp(const char *s1, const char *s2, int n)
{
	while(*s1 && --n && (*s1 == *s2))
	{
		s1++;
		s2++;
	}

	return (s1[0] - s2[0]);
}

int strlen(const char *s)
{
	const char *t = s;
	while(*t++);
	return t-s-1;
}

char *strcpy(char *dst, const char *src)
{
	register char *d = dst;
	while(*src)
		*dst++ = *src++;
	*dst++ = 0;
	return d;
}

char *strncpy(char *dst, const char *src, int n)
{
	register char *d = dst;
	while(*src && n--)
		*dst++ = *src++;
	*dst++ = 0;
	return d;
}

extern volatile int frame_counter;

int clock(void)
{
	return frame_counter;
}

#define UNBCD(x) (((x) & 0xF) + (((x) >> 4) * 10))


struct tm *time2(struct tm *tm)
{
	uchar data[7];

	rtc_get(data);

	tm->tm_sec = UNBCD(data[6]);
	tm->tm_min = UNBCD(data[5]);

	tm->tm_hour = UNBCD(data[4] & 0x3F);

	//fprintf(2, "%02x %02x\n", data[2], data[4]);

	if(data[4] & 0x80)
		tm->tm_hour += 12;

	tm->tm_mday = UNBCD(data[2] & 0x3F);
	tm->tm_mon = UNBCD(data[1]);
	tm->tm_year = UNBCD(data[0]) + 2000;

	return tm;
}

static struct tm tmp_tm;

#define SECpMIN 60
#define SECpHOUR 3600
#define SECpDAY (3600*24)
#define SECpMON (3600*24*31)
#define SECpYEAR (3600*24*31*12)

struct tm *localtime(time_t t)
{
	tmp_tm.tm_year = t / SECpYEAR;
	t -= (tmp_tm.tm_year * SECpYEAR);

	tmp_tm.tm_year += 2000;

	tmp_tm.tm_mon = t / SECpMON;
	t -= (tmp_tm.tm_mon * SECpMON);

	tmp_tm.tm_mday = t / SECpDAY;
	t -= (tmp_tm.tm_mday * SECpDAY);

	tmp_tm.tm_hour = t / SECpHOUR;
	t -= (tmp_tm.tm_hour * SECpHOUR);

	tmp_tm.tm_min = t / SECpMIN;
	t -= (tmp_tm.tm_min * SECpMIN);

	tmp_tm.tm_sec = t;


	return &tmp_tm;
}


time_t time(time_t *t)
{
	//char tmp[64];

	uint32 total;
	int secs,mins,hour,days,mons,year;
	uchar data[7];

	rtc_get(data);

	//sprintf(tmp, "%x %x %x %x %x %x\n", data[0], data[1], data[2], data[4], data[5], data[6]);
	//dprint(tmp);

	secs = UNBCD(data[6]);
	mins = UNBCD(data[5]);
	hour = UNBCD(data[4]);

	days = UNBCD(data[2]);
	mons = UNBCD(data[1]);
	year = UNBCD(data[0]);

	total = secs + mins*SECpMIN + hour*SECpHOUR + days*SECpDAY + mons*SECpMON + year*SECpYEAR;
	if(t) *t = total;

	//sprintf(tmp, "%d %d %d - %02d:%02d:%02d (%08x)\n", year + 2000, mons, days, hour, mins, secs, total);
	//dprint(tmp);

	return total;

}
