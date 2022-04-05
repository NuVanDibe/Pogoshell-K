#ifndef RTC_H
#define RTC_H

void rtc_enable(void);
int rtc_get(unsigned char *data);
int rtc_check(void);

#endif
