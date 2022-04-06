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
