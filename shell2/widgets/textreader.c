
#include <pogo.h>

#include "../window.h"
#include "textreader.h"
#include "../users.h"

#define VRAM ((uint16 *)0x06000000)

extern int CurrentUser;

// Offsets for lines of current page
int lines[25];

// Offset for each page in text
static int *pages = (int *)(0x02000000 + 256*1024 - 1024*4);
static char *linebuf = (char *)(0x02000000 + 256*1024 - 1024*4 - 4096);

int page_count = 0;


Typeface *typeface_list[4];

TextReader treader;

void textreader_init()
{
	int i;
	for (i = 0; i < 4; i++)
		typeface_list[i] = NULL;
}

void textreader_set_typeface(int n, Typeface *tf)
{
	if (typeface_list[n] && !typeface_list[n]->global) {
		if (typeface_list[n]->font)
			free(typeface_list[n]->font);
		free(typeface_list[n]->font);
	}
	typeface_list[n] = tf;
}

void textreader_set_font(int n, Font *f)
{
	if (typeface_list[n] && !typeface_list[n]->global) {
		if (typeface_list[n]->font)
			free(typeface_list[n]->font);
		free(typeface_list[n]->font);
	}
	typeface_list[n] = typeface_new(f, 0);
}

// Seeks to beginning of next word, return pixel length
int next_word(Font *font, char **text)
{
	char *p = *text;
	int l,ff = font->first;
	int w = 0;

	while(*p > ' ')
	{
		if(*p <= font->last)
		{
			if(font->flags & FFLG_PROPORTIONAL)
				l = font->offsets[*p - ff + 1] - font->offsets[*p - ff] + font->spacing;
			else
				l = font->charwidth;
		} else
			l = 0;

		w += l;
		p++;
	}

	while(*p == ' ')
	{
		if(w+font->charwidth > 232)
		{
			*text = p;
			return w;
		}
		w += (font->charwidth + font->spacing);
		p++;
	}

	*text = p;
	return w;
}

int typeface_text_multi(Typeface **typefacelist, int *current, char *str, uint16 *dest, int width)
{
	Typeface *tf;
	uint16 *outw = dest;

	tf = typefacelist[*current];
	font_setshadowoutline(TO_RGB16(tf->shadow), TO_RGB16(tf->outline));

	while(*str)
	{
		if(*str <= 8) {
			tf = typefacelist[*current = (*str++ - 1)];
			font_setshadowoutline(TO_RGB16(tf->shadow), TO_RGB16(tf->outline));
		} else
			dest += font_putchar(tf->font, *str++, dest, width);
	}
	return dest-outw;
}



/*
  10 counts as newline, 13 is ignored
  line-indexes point to after the 10 and the 13 if present
*/

#define PAR_SIZE 4096


// Scan forward one page in the file. A page is 'height' lines of text
// Returns 0 if not EOF, otherwise number of lines before EOF (always < 'height')
int scan_page(FILE *fp, int height)
{
	int l;
	int fset;
	Font *font;

	int c;
	char *ptr, *lastp = NULL;
	int len, w;
	int lc = 0;
	
	font = typeface_list[FONT_TEXT]->font;

	c = 0;
	while(c != EOF)
	{
		// Scan one line (paragraph)
		ptr = linebuf;
		len = 0;
		c = 0;

		lines[lc++] = ftell(fp);
		if(lc == height+1)
			return 0;

		// Read complete line to 'linebuf'
		while((c != 10) && (c != EOF) && (len < PAR_SIZE))
		{
			len++;
			c = getc(fp);
			*ptr++ = c;
		}
		ptr--;
		*ptr = 0;
		ptr = linebuf;

		// 'linebuf' = 'ptr' = null-terminated complete line to be wrapped
		// 'len' = length of that line

		while(*ptr)
		{
			w = 0;
			//  Read all words in line
			while(w < 232 && *ptr)
			{
				fset = 0;
				// Skip/check unprintables
				while(*ptr && *ptr < ' ')
				{
					// If font has not been set this word and this is a font set command
					if(!fset && *ptr <= 8)
					{
						fset = 1;
						font = typeface_list[*ptr - 1]->font;
					}
					ptr++;
				}
				
				// Starting with actual word (after any special chars parsed above)
				if(*ptr)
				{
					lastp = ptr;
					l = next_word(font, &ptr);

					// now:
					// lastp = start of word
					// ptr = end of word

					// Special case: if the first word on the line was too long...
					if(!w && l > 232)
						lastp = ptr;

					w += l;

					// If a space made us pass the border, subtract the width of the space
					// (shouldnt be drawn at the end of a line)
					if(w >= 232 && ptr[-1] == ' ')
						w -= font->charwidth;
				}
			}

			if(w >= 232)
			{
				// We passed screen border

				// Subtract length of last word (because it must be printed on next line)
				w -= l;

				ptr = lastp;
				// Store the offset
				// filepointer is at the end of the unwrapped line
				// subtract len to get to start of unwrapped line
				// (ptr - linebuf) is the current offset into that line
				lines[lc++] = ftell(fp) - len + (ptr - linebuf);

				if(lc == height+1)
				{
					// If end of page, return 0
					fseek(fp, lines[lc-1], SEEK_SET);
					return 0;
				}

				w -= 240;

			}
		}
	}
	lines[lc] = ftell(fp);
	return lc;
}

enum { AL_LEFT, AL_CENTER, AL_RIGHT };

void block_set(uint16 *dest, int width, int height, int dwidth, uint16 v);

extern void dprint(const char *sz);
void make_savename(char *dst, char *src, char *ext);

void save_index(char *name)
{
	char tmp[40];
	unsigned short c;
	make_savename(tmp, name, ".idx");
	int i;
	FILE *fp;

	sram_setuser(100);
	if((fp = fopen(tmp, "wb")))
	{
		for(i=0; i<page_count; i++)
		{
			c = pages[i] & 0xFFFF;
			fwrite(&c, 2, 1, fp);
		}
		fclose(fp);
	}
	sram_setuser(CurrentUser);
}

int load_index(char *name)
{
	unsigned short c, lastc;
	char tmp[40];
	int total = 0;
	FILE *fp;
	make_savename(tmp, name, ".idx");
	
	sram_setuser(100);

	if((fp = fopen(tmp, "rb")))
	{
		page_count = 0;
		lastc = 0;
		while(fread(&c, 2, 1, fp) > 0)
		{
			if(c < lastc) total += 0x10000;
			pages[page_count] = total | c;
			lastc = c;
			page_count++;
		}
		fclose(fp);
		sram_setuser(CurrentUser);
		return 1;
	}
	sram_setuser(CurrentUser);
	return 0;
}

int textreader_show_text(char *name)
{
	//uchar *bmark_ptr;
	int modifiers = 0;
	int align = 0;
	uint16 *vp;
	int rc,c,i,j,l,lastl;
	int lin,lastlin;
	//int last_lastfind = 0;
	int lastfind = 0;
	char linebuf[120];
	int height;
	int offset,done = 0;
	Font *font;
	FILE *fp, *fp2;

	font = typeface_list[FONT_TEXT]->font;

	uint16 bg = 0x7FFF, fg = 0x0000, shadow, outline;

	shadow = TO_RGB16(typeface_list[FONT_TEXT]->shadow);
	outline = TO_RGB16(typeface_list[FONT_TEXT]->outline);

	font_setcolor(fg, bg);
	font_setshadowoutline(shadow, outline);

	height = (160 / font_height(font)) - 1;

	page_count = 0;

	block_set(VRAM, 240, 160, 240, bg);

	offset = ((160 - font_height(font) * (height+1)) / 2 ) * 240 + 4;

	if((fp = fopen(name, "rb")))
	{
		if(!load_index(name))
		{
			pages[page_count++] = ftell(fp);
			while(!scan_page(fp, height))
			{
				pages[page_count++] = ftell(fp);
				if(!(page_count % 10))
				{
					sprintf(linebuf, "\372INDEXING %03d (%d)    ", page_count, pages[page_count-1]);
					font_text(font, linebuf, VRAM, 240);
					if(getchar() == RAWKEY_B)
						return 0;
				}
			}
			save_index(name);
		}
	}
	else
		return -1;

	l = 0;
	fseek(fp, pages[0], SEEK_SET);

	if((fp2 = fopen("/sram/.bookmark", "rb")))
	{
		unsigned int l2;
		fread(&l2, 1, 4, fp2);
		l = (int)(l2 & 0x7FFFFFFF);
		fread(linebuf, 1, sizeof(linebuf), fp2);
		fclose(fp2);
		if(strcmp(linebuf, name) == 0)
			fseek(fp, pages[l], SEEK_SET);
		else
			l = 0;
		
	}

	lin = 0;
	lastl = -1;
	lastlin = -1;

	while(!done)
	{
		if((lastl != l) || (lastlin != lin))
		{
			c = scan_page(fp, height);

			if(!c)
				c = height;

			vp = VRAM;
			i = 0;
			if(lastl == l)
			{
				i = height-1;
				vp = (VRAM + (font_height(font) * 240 * i));
			}

			block_set(vp, 240, 160-font_height(font), 240, bg);
			vp += offset;
			for(; i<height; i++)
			{
				if(i < c)
				{
					fseek(fp, lines[i], SEEK_SET);
					fread(linebuf, rc = lines[i+1] - lines[i], 1, fp);

					linebuf[rc] = 0;

					for(j=0; j<rc; j++)
					{
						switch(linebuf[j])
						{
						case 11:
							align = AL_CENTER;
							break;
						case 14:
							align = AL_RIGHT;
							break;
						case 13:
							align = AL_LEFT;
							break;
						}
					}

					j = lastfind;
					rc = typeface_text_multi(typeface_list, &j, linebuf, NULL, 240);

					switch(align)
					{
					case AL_RIGHT:
						typeface_text_multi(typeface_list, &lastfind, linebuf, (vp+(236-rc)), 240);
						break;
					case AL_LEFT:
						typeface_text_multi(typeface_list, &lastfind, linebuf, vp, 240);
						break;
					case AL_CENTER:
						typeface_text_multi(typeface_list, &lastfind, linebuf, (vp+((236-rc)/2)), 240);
						break;
					}
				}

				vp += (font_height(font) * 240);
			}
			lastl = l;
			lastlin = lin;
			vp = (unsigned short *)(0x06000000 + ((160 - font_height(font)) * 480));
			for(c=0; c<240*font_height(font); c++)
				vp[c] = fg;

			font_setcolor(bg, fg); //txbgcolor, txfgcolor);
			font_setshadowoutline(outline, shadow);
			sprintf(linebuf, "PAGE %03d/%03d * LINE %04d/%04d", l+1, page_count, lin + l*height + 1, page_count*height);
			font_text(font, linebuf, (vp+2), 240);
			font_setcolor(fg, bg); //txfgcolor, txbgcolor);
		}

		while((c = getchar()) == EOF)
			Halt();
		switch(c)
		{
		case RAWKEY_L:
			modifiers |= 1;
			break;
		case RAWKEY_R:
			modifiers |= 2;
			break;
		case RAWKEY_L | 0x80:
			modifiers &= ~1;
			break;
		case RAWKEY_R | 0x80:
			modifiers &= ~2;
			break;
		case RAWKEY_START:

		case RAWKEY_UP:
			if(modifiers == 1)
				l -= 10;
			else
				l -= 1;
			break;
		case RAWKEY_A:
/*			fseek(fp, lines[1], SEEK_SET);
			scroll(font_height(font));
			lin++;
			break;*/
		case RAWKEY_DOWN:
			if(modifiers == 1)
				l += 10;
			else
				l += 1;
			break;
		case RAWKEY_LEFT:
			if(modifiers == 1)
				lin = l = 0;
			else
				l -= 50;
			break;
		case RAWKEY_RIGHT:
			if(modifiers == 1)
			{
				l = page_count-1;
				lin = 0;
			}
			else
				l += 50;
			break;
		case RAWKEY_B:
			done = 1;
			break;
		case RAWKEY_SELECT:
			///gui_messagebox(NULL, TEXT(TREAD_HELP_TITLE), TEXT(TREAD_HELP_TEXT), 0);
			lastl = -1;
			break;
		}

		if(lin >= height)
		{
			lin = 0;
			l++;
		}

		if(l<0) l = 0;
		if(l>=page_count) l = page_count-1;

		if(lastl != l)
		{
			fseek(fp, pages[l], SEEK_SET);
			lin = 0;

			if((fp2 = fopen("/sram/.bookmark", "wb")))
			{
				unsigned int l2 = (unsigned int)l | 0x80000000;
				fwrite(&l2, 1, 4, fp2);
				fwrite(name, 1, strlen(name)+1, fp2);
				fclose(fp2);
			}
		}
	}

	fclose(fp);

	if((fp2 = fopen("/sram/.bookmark", "wb")))
	{
		fwrite(&l, 1, 4, fp2);
		fwrite(name, 1, strlen(name)+1, fp2);
		fclose(fp2);
	}

	return 0;
}

void textreader_set_attribute(TextReader *tr, int attr, void *val)
{

}

TextReader *textreader_new()
{

	//render_functions[WIDGET_TEXTREADER & 0xFFF] = (WidgetRenderFunc)textreader_render;

	treader.w.type = WIDGET_TEXTREADER;
	treader.w.height = 160;
	treader.w.width = 240;
	treader.w.flags = WFLG_REDRAW;


	return &treader;
}

