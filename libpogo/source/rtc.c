#include <pogo.h>

#define RTC_DATA ((volatile uint16 *)0x080000C4)
#define RTC_RW ((volatile uint16 *)0x080000C6)
#define RTC_ENABLE ((volatile uint16 *)0x080000C8)

void rtc_cmd(int v)
{
	int l;
	uint16 b;

	v = v<<1;

	for(l=7; l>=0; l--)
	{
		b = (v>>l) & 0x2;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 5;
	}
}

void rtc_data(int v)
{
	int l;
	uint16 b;

	v = v<<1;

	for(l=0; l<8; l++)
	{
		b = (v>>l) & 0x2;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 4;
		*RTC_DATA = b | 5;
	}
}

int rtc_read(void)
{
	int j,l;
	uint16 b;
	int v = 0;


	for(l=0; l<8; l++)
	{
		for(j=0;j<5; j++)
			*RTC_DATA = 4;
		*RTC_DATA = 5;
		b = *RTC_DATA;
		v = v | ((b & 2)<<l);
	}

	v = v>>1;

	return v;
}

static int check_val = 0;

void rtc_enable(void)
{
	*RTC_ENABLE = 1;

	*RTC_DATA = 1;
	*RTC_DATA = 5;
	*RTC_RW = 7;

	rtc_cmd(0x63);

	*RTC_RW = 5;

	check_val =  rtc_read();
}

// Normally returns 0x40
int rtc_check(void)
{
	return (check_val != 0);
}

int rtc_get(char *data)
{
	int i;

/*	*RTC_DATA = 1;
	*RTC_DATA = 5;
	*RTC_RW = 7;

	rtc_cmd(0x60);

	*RTC_DATA = 1;
	*RTC_DATA = 5;
	*RTC_RW = 7;

	rtc_cmd(0x62);
	rtc_data(0x40);
*/
	*RTC_DATA = 1;
	*RTC_RW = 7;

	*RTC_DATA = 1;
	*RTC_DATA = 5;

	rtc_cmd(0x65);

	*RTC_RW = 5;

	for(i=0; i<4; i++)
		data[i] = (char)rtc_read();
	
	*RTC_RW = 5;

	for(i=4; i<7; i++)
		data[i] = (char)rtc_read();

	return 0;
}
