
#ifndef LISTVIEW_H
#define LISTVIEW_H

#include "scrollbar.h"
#include "../backdrop.h"

#define MAPSLOTCOUNT ((MAX_FILE_COUNT+31)>>5)

#define WIDGET_LISTVIEW 0x1003
typedef struct
{
	Widget w;

	BackDrop *backdrop;

	uint16 columns;
	uint16 lines;  // Current number of line in list
	int16 marked;  // Marked line, -1 = no marked line
	int16 start;   // First line showing
	uint16 lineh;  // Pixelheight of lines, recalced when font or icons are set
	uint16 linew;  // Length of longest line
	uint16 iconw;  // Width of largest icons
	uint16 marginx; // Indentation from left (in pixels)
	uint16 marginy;
	uint16 showing;

	uchar dirty;   // Redraw?
	uint32 redrawmap[MAPSLOTCOUNT];

	Font *font;

	uint16 colwidth[8];
	uint16 colalign[8];

	BitMap **icons;

	char **texts[8];

	Scrollbar *scrollbar;
	
	Color textcolor[4];

} ListView;

#define listview_get_marked(l) (l->marked)

ListView *listview_new(int columns, int maxlines, Font *font);
void listview_addline(ListView *lv, BitMap *icon, ...);
void listview_setline(ListView *lv, int index, BitMap *bm, ...);
void listview_set_attribute(ListView *tb, int attr, void *val);
void listview_set_marked(ListView *lv, int i);
void listview_set_dirty(ListView *lv);

void listview_clear(ListView *lv);
void listview_set_start(ListView *lv, int i);

#endif

