#ifndef FONT_H
#define FONT_H

typedef struct {
	uchar flags;
	signed char spacing;
	uchar first;
	uchar last;
	uchar charwidth;	// Only for space in proportional fonts
	uchar height;		// Height in pixels
	uint16 width;		// Real width of entire font in pixels - actual planar data is aligned up to nearest 32

	/* Non-disk part */
	char *name;
	uint16 *colors;
	uchar *pixels;		// Chunky data, width*height bytes
	uint16 *offsets;	// NULL for monospaced font
} Font;

/* Disk format

	- Font structure (First part)
	- width*height/8 bytes planar pixels
	- (last-first) 16bit offsets
*/

#define FFLG_PROPORTIONAL 1
#define FFLG_HIFONT 2
#define FFLG_COLOR 4
#define FFLG_TRANSP 8
#define FFLG_BOLD 16
#define FFLG_ITALIC 32


uchar font_putchar(Font *font, char c, uint16 *dest, int width);
int font_text(Font *font, char *str, uint16 *dest, int width);
int font_text_multi(Font **fontlist, int *current, char *str, uint16 *dest, int width);

//uchar font_putchar(Font *font, char c, uchar *dest, int width);
//int font_text(Font *font, char *str, uchar *dest, int width);
//int font_text_multi(Font **fontlist, int *current, char *str, uchar *dest, int width);
void font_setcolor(uint16 fg, uint16 bg);
Font *font_load(char *name);
Font *font_dup(Font *font);

#endif
