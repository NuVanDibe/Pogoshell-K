#ifndef SETTINGS_H
#define SETTINGS_H

#include "bitmap.h"

//int settings_read(void);
//int settings_write(void);
void settings_edit(void);
void settings_icon(BitMap *bm);
void settings_init(void);
void settings_default(void);
void get_theme_name(char line, char *dest);
int set_theme_setting(char *src);

enum { SF_HIDEEXT, SF_ASKSAVE, SF_ASKLOAD, SF_HIDEDOT, SF_INTROSKIP,
       SF_HIDESIZE, SF_SORTING, SF_SCROLLBAR, SF_SLEEP, SF_SCREENSAVER,
       SF_THEME,
       NO_SETTINGS };

enum {SORT_NAME, SORT_TYPE, SORT_SIZE, SORT_NONE};

extern unsigned /*short*/char settings[NO_SETTINGS];

#define settings_get(i) (settings[i])
#define settings_set(i, v) do { settings[i] = v; } while(0)

#endif
