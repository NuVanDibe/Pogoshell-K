
#include <pogo.h>

#include "../window.h"
#include "trp.h"
#include "../users.h"
#include "../unapack.h"

#define VRAM ((uint16 *)0x06000000)

#define MAGIC (0x5848681a) //0x1a, "XTX"
#define AVAILABLE (128*1024)
#define CACHE 2

// Cache to store decompressed blocks
static char *cache = (int *)(0x02000000 + 256*1024 - AVAILABLE);
u32 cache_start = 0;
u32 cache_count = CACHE;

// Compressed blocks
uchar *file;
u32 size, block_size, block_count;
uchar *blocks;

int line_starts[160];

static Font *font;

TextReaderPlus treaderp;

void block_set(uint16 *dest, int width, int height, int dwidth, uint16 v);
extern void dprint(const char *sz);

void textreaderplus_set_font(int n, Font *f)
{
	font = f;
}

void update_cache(int offset)
{
	u32 tmp;

	if (offset >= cache_start && offset < cache_start + block_count*block_size)
		return;
	tmp = offset/block_size;
	if (tmp + 1 == cache_start) {
		cache_start = tmp;
		memcpy(&cache[block_size], cache, block_size);
		depack(file+blocks[cache_start], cache);
	} else if (tmp - 2 >= 0 && tmp - 2 == cache_start) {
		cache_start = tmp - 1;
		memcpy(cache, &cache[block_size], block_size);
		depack(file+blocks[cache_start+1], &cache[block_size]);
	} else {
		if (tmp == block_count - 1 || offset%block_size < (block_size>>1))
			cache_start = tmp - 1;
		else
			cache_start = tmp;
		depack(file+blocks[cache_start], cache);
		depack(file+blocks[cache_start+1], &cache[block_size]);
	}
}

int textreaderplus_show_text(char *name)
{
	u16 *screen;
	int modifiers = 0;
	int align = 0;
	int linestart, lineend;
	char bookmark[48];
	int height, old_offset, offset, done = 0;
	FILE *fp;
	unsigned short bg = 0x7FFF;
	unsigned short fg = 0x0000;
	u16 chapter_count;
	u32 *chapters, chapter = 0;
	int line, i;
	int width, w, drawwidth;
	int ro, relative_offset, relative_size;
	uchar *text;
	int rate = 0;
	int lastpage;

	u16 linebuf[font->height][240];

	file = file2mem(name, NULL, 0);

	if (!file)
		return 1;

	if (*(u32 *)file != MAGIC)
		return 1;

	size = *(u32 *)(file+4);
	block_size = *(u32 *)(file+8);
	chapter_count = *(u16 *)(file+16);
	blocks = (uchar **)(file+18);

	block_count = size/block_size;
	if (size%block_size)
		block_count++;
	chapters = blocks + sizeof(uchar **) * page_count;

	if (chapter_count) {
		for (i = 1; i < chapter_count; i++)
			if (chapter_count[i-1] >= chapter_count[i])
				return 1;
		if (chapter[chapter_count-1] > size)
			return 1;
	}

	if ((block_size<<1) > AVAILABLE)
		return 1;

	strcpy(bookmark, "/sram/");
	strcat(bookmark, name);
	strcat(bookmark, ".bookmark");
	if((fp = fopen(bookmark, "rb"))) {
		fread(&offset, 1, 4, fp);
		fclose(fp2);
		offset &= 0x7FFFFFFF;
		if (offset > size)
			return 1;
	}

	font_setcolor(fg, bg);

	height = (160 / font->height) - 1;

	while(!done)
	{
		old_offset = offset;
		block_set(VRAM, 240, 160, 240, bg);
		update_cache(offset, blocks, size);
		screen = VRAM;
		ro = relative_offset = offset - cache_start * block_size;
		relative_size = size - cache_start * block_size;
		linestart = offset;
		line = 0;
		while (ro < relative_size && line < height) {
			text = &cache[relative_offset];
			lineend = linestart;
			width = 240;
			w = 0;
			while (width && ro < relative_size)
			{
				if (*text == '\n') {
						lineend = ro - relative_offset + offset;
						break;
				}
				l = font_putchar_clip(font, *text, &linebuf[w], 240, width);
				if (*text == ' ' && t) {
					t = 0;
					lineend = ro - relative_offset + offset;
					drawwidth = w;
				} else if (*text != ' ' && l)
					t = 1;
				width -= l;
				w += l;
				text++;
				ro++;
			}
			linestart[line] = linestart;
			lineend[line] = lineend;

			block_copy(screen, linebuf, drawwidth, font->height, 240, 1);
			screen += font->height * 240;

			linestart = ro - relative_offset + offset;
			line++;
		}
		line_start[line] = linestart;

		while((c = getchar()) == EOF)
			Halt();

		switch(c)
		{
			case RAWKEY_UP:
				switch(rate)
				{
					case 1:
						break;
					case 2:
						for (i = 0; i < chapter_count; i++)
						{
							if (chapters[i] > offset)
								break;
						}
						i--;
						if (chapter_count && i >= 0)
							offset = chapters[i];
						break;
					default:
						break;
				}
				break;
			case RAWKEY_DOWN:
				switch(rate)
				{
					case 1:
						lastpage = 0;
						for (i = 0; i < height; i++)
						{
							if (line_start[i] >= size) {
								lastpage = 1;
								break;
							}
						}
						if (!lastpage)
							offset = line_start[height];
						break;
					case 2:
						for (i = 0; i < chapter_count; i++)
						{
							if (chapters[i] > offset) {
								offset = chapters[i];
								break;
							}
						}
						break;
					default:
						lastpage = 0;
						for (i = 0; i < height; i++)
						{
							if (line_start[i] >= size) {
								lastpage = 1;
								break;
							}
						}
						if (!lastpage)
							offset = line_start[1];
						break;
				}
				break;
			case RAWKEY_LEFT:
				if (rate > 0)
					rate--;
				break;
			case RAWKEY_RIGHT:
				if (rate < 2)
					rate++;
				break;
			case RAWKEY_B:
				done = 1;
				break;
		}

		if (offset != old_offset) {
			if((fp = fopen(bookmark, "wb")))
			{
				fwrite(&offset, 1, 4, fp2);
				fclose(fp);
			}
		}
	}

	if((fp = fopen(bookmark, "wb")))
	{
		fwrite(&offset, 1, 4, fp2);
		fclose(fp);
	}

	return 0;
}

void textreaderplus_set_attribute(TextReader *tr, int attr, void *val)
{

}

TextReaderPlus *textreaderplus_new()
{

	//render_functions[WIDGET_TEXTREADER & 0xFFF] = (WidgetRenderFunc)textreader_render;

	treaderp.w.type = WIDGET_TEXTREADER;
	treaderp.w.height = 160;
	treaderp.w.width = 240;
	treaderp.w.flags = WFLG_REDRAW;

	return &treaderp;
}

