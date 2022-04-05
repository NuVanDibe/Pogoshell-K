#ifndef CARTLIB_H
#define CARTLIB_H

#define CARTLIB

#define FC_CART_SUPPORTED       1
#define FC_ROMSWITCH_SUPPORT    2
#define FC_SRAMSWICH_SUPPORT    4
#define FC_RTC_SUPPORT          8


char *fcGetCartName(void);
unsigned int fcGetSupport(void);
int fcGetCartSize(void);
int fcGetSRamSize(void);
void fcSetSRamStart(unsigned int offset);
void fcExecuteRom(unsigned int offset, int flags);
int fcGetRTC(void);
void fcSetRTC(void);

#endif
