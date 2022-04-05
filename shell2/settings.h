#ifndef SETTINGS_H
#define SETTINGS_H

#include "bitmap.h"

//int settings_read(void);
//int settings_write(void);
/*int*/void settings_edit(void);
void settings_icon(BitMap *bm);
void settings_init(void);

enum { SF_HIDEEXT, SF_ASKSAVE, SF_ASKLOAD, SF_HIDEDOT, SF_INTROSKIP,
       SF_HIDESIZE, SF_SORTING, SF_SCROLLBAR, /*SF_THEME,*/
       NO_SETTINGS };

enum {SORT_NAME, SORT_TYPE, SORT_SIZE};

extern unsigned /*short*/char settings[NO_SETTINGS];

#define settings_get(i) (settings[i])

#endif
