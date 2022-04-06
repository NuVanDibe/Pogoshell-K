/*
 * Font handling functions (font.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Functions for writing text with different fonts into
 * an 8bit VRAM mode.
 *
 **/

#include "core.h"
#include "device.h"
#include "font.h"
#include "io.h"

enum {BG, FG, SHADOW, OUTLINE};

// Current set of colors for non-color fonts
static uint16 font_colors[16];

static uint16 *colors;

void font_setcolor(uint16 fg, uint16 bg)
{
	font_colors[FG] = fg;
	font_colors[BG] = bg;
}

void font_setshadowoutline(uint16 shadow, uint16 outline)
{
	font_colors[SHADOW] = shadow;
	font_colors[OUTLINE] = outline;
}

static void block_copy(uint16 *dst, uchar *src, int width, int height, int sw, int dw, int solid)
{
	int w;
	int smod = (sw - width);
	int dmod = (dw - width);
	uint16 fg;

	if (colors == font_colors) {
		fg = colors[FG];

		while(height--)
		{
			w = width;
			while(w--)
			{
				if(*src)
					*dst = fg;
				dst++;
				src++;
			}
			dst += dmod;
			src += smod;
		}
	} else {
		while(height--)
		{
			w = width;
			while(w--)
			{
				if(*src)
					*dst = colors[*src];
				dst++;
				src++;
			}
			dst += dmod;
			src += smod;
		}
	}
}


void block_set(uint16 *dst, int width, int height, int dw, int color)
{
	int w;
	int dmod = (dw - width);

	while(height--)
	{
		w = width;
		while(w--)
			*dst++ = color;
		dst += dmod;
	}
}

static uchar font_putmono(Font *font, char c, uint16 *dest, int width, int dw)
{
	int w;
	int solid = !(font->flags & FFLG_TRANSP);

	w = font->charwidth;
	if(dest)
	{
		if (c == ' ') {
			if (solid)
				block_set(dest, (w < dw) ? w : dw, font->height, width, colors[BG]);
		} else {
			if (c < font->first || c >= font->last)
				return 0;

			if (solid)
				block_set(dest, (w < dw) ? w : dw, font->height, width, colors[BG]);
			block_copy(dest, &font->pixels[(c - font->first) * w], (w < dw) ? w : dw, font->height, font->width, width, solid);
		}
	}
	return w;
}

static uchar font_putprop(Font *font, char c, uint16 *dest, int width, int dw)
{
	int offset, w;
	int solid = !(font->flags & FFLG_TRANSP);
	int ff = font->first;

	if(c == ' ')
	{
		w = font->charwidth;
		if(dest && solid)
			block_set(dest, (w < dw) ? w : dw, font->height, width, colors[BG]);
	} 
	else
	{
		if(c < ff || c >= font->last)
			return 0;

		offset = font->offsets[c - ff];
		w = font->offsets[c - ff + 1] - offset;

		if(dest)
		{
			if(solid)
				block_set(dest, (w < dw) ? w : dw, font->height, width, colors[BG]);
			block_copy(dest, &font->pixels[offset], (w < dw) ? w : dw, font->height, font->width, width, solid);
		}
		w += font->spacing;
		if (w <= 0) w = 1;
	}
	return w;
}

uchar font_putchar_clip(Font *font, char c, uint16 *dest, int width, int drawwidth)
{
	uchar (*putchar)(Font *, char, uint16 *, int, int);
	int dw = drawwidth;
	int w, w2, h, h2;
	int solid = !(font->flags & FFLG_TRANSP),
		bold = (font->flags & FFLG_BOLD),
		outline = (font->flags & FFLG_OUTLINE),
		dropshadow = (font->flags & FFLG_DROPSHADOW);
	uint16 old_fg;
	int old_flags;

	w2 = 0;
	h2 = 0;

	if (outline) {
		w2 += 2;
		h2 += 2;
	}

	if (bold)
		w2++;

	if (dropshadow) {
		w2++;
		h2++;
	}

	if(font->offsets)
		putchar = font_putprop;
	else
		putchar = font_putmono;

	old_flags = font->flags;
	colors = font_colors;

	w = putchar(font, c, NULL, width, dw);
	h = font->height;

	w2 += w;
	h2 += h;

	if (dest) {
		if (outline || dropshadow || bold) {
			if (solid) {
				block_set(dest, (w2 < dw) ? w2 : dw, h2, width, colors[BG]);
				font->flags &= FFLG_TRANSP;
			}
		}

		if (dropshadow) {
			old_fg = colors[FG];
			colors[FG] = colors[SHADOW];

			dest += (width + 1);
			if (outline) {
				/*
				putchar(font, c, dest + (width<<1), width, --dw);
				dest++;
				putchar(font, c, dest + (width<<1), width, --dw);
				dest++;
				if (bold) {
					putchar(font, c, dest + (width<<1), width, --dw);
					dest++;
				}
				putchar(font, c, dest, width, dw - 1);
				putchar(font, c, dest + width, width, dw - 1);
				putchar(font, c, dest + (width<<1), width, dw - 1);
				if (bold) {
					dest--;
					dw++;
				}
				dw += 2;
				dest -= 2;
				*/
				dest++;
				--dw;
				if (bold) {
					putchar(font, c, dest + (width<<1), width, --dw);
					dest++;
				}
				putchar(font, c, dest + width, width, --dw);
				putchar(font, c, dest + (width<<1), width, dw);
				dest++;
				putchar(font, c, dest + width, width, --dw);
				if (bold) {
					dest--;
					dw++;
				}
				dw += 2;
				dest -= 2;
			} else {
				putchar(font, c, dest, width, --dw);
				if (bold)
					putchar(font, c, dest + 1, width, dw - 1);
				dw++;
			}
			dest -= (width + 1);
			colors[FG] = old_fg;
		}

		if (outline) {
			old_fg = colors[FG];
			colors[FG] = colors[OUTLINE];

			//putchar(font, c, dest, width, --dw);
			putchar(font, c, dest + width, width, dw);
			//putchar(font, c, dest + (width<<1), width, dw);
			dest++;
			putchar(font, c, dest, width, --dw);
			putchar(font, c, dest + (width<<1), width, dw);
			dest++;
			if (bold) {
				putchar(font, c, dest, width, --dw);
				putchar(font, c, dest + (width<<1), width, dw);
				dest++;
			}
			//putchar(font, c, dest, width, --dw);
			putchar(font, c, dest + width, width, dw);
			//putchar(font, c, dest + (width<<1), width, dw);
			if (bold) {
				dest--;
				dw++;
			}
			dw += 2;
			dest += (width - 1);
			colors[FG] = old_fg;
		}

		if(font->flags & FFLG_COLOR)
			colors = font->colors;
		else
			colors = font_colors;

		putchar(font, c, dest, width, dw);
		drawwidth--;
		if (bold)
			putchar(font, c, dest + 1, width, dw);
		font->flags = old_flags;
	}

	return w2;
}

int font_height(Font *font)
{
	int h2;
	int outline = (font->flags & FFLG_OUTLINE),
		dropshadow = (font->flags & FFLG_DROPSHADOW);

	h2 = font->height;

	if (outline)
		h2 += 2;

	if (dropshadow)
		h2++;

	return h2;
}

uchar font_putchar(Font *font, char c, uint16 *dest, int width)
{
	return font_putchar_clip(font, c, dest, width, width);
}

#if 0
int font_text_clip(Font * font, char *str, uint16 * dest, int width, int drawwidth)
{
	int l = 0, w = 0;

    while (drawwidth > 0 && *str) {
		l = font_putchar_clip(font, *str++, dest, width, drawwidth);
		w += l;
		drawwidth -= l;
    }
	return w;
}
#endif

int font_text_truncate(Font * font, char *str, uint16 * dest, int width, int drawwidth)
{
    int l, period;
    char *s = str, *s1, *s2;
	int left, right;

	period = font_putchar(font, '.', NULL, width);
	period += period<<1;

	right = (drawwidth-period)>>1;
	right = (right < 0) ? 0 : right;
	left = right;

	l = 0;
    // Find left side.
    while (*s) {
		l += font_putchar(font, *s++, NULL, width);
		if (left && l > left) {
			s1 = s - 1;
			left = 0;
		}
		if (l > drawwidth)
			break;
    }

    // Compensate if too wide.
	if (l > drawwidth) {
		s = str + strlen(str) - 1;
		l = 0;
		// Find right side.
    	while (s > str) {
			l += font_putchar(font, *s--, NULL, width);
			if (l > right) {
				s2 = s + 2;
				break;
			}
		}
		// Draw left.
		l = 0;
		while (str < s1)
			l += font_putchar(font, *str++, dest ? &dest[l] : NULL, width);
		// Draw middle.
    	l += font_putchar(font, '.', dest ? &dest[l] : NULL, width);
    	l += font_putchar(font, '.', dest ? &dest[l] : NULL, width);
    	l += font_putchar(font, '.', dest ? &dest[l] : NULL, width);
		// Draw right.
		str = s2;
		while (*str)
		    l += font_putchar(font, *str++, dest ? &dest[l] : NULL, width);
    } else {
		// Draw whole str.
		l = 0;
		while (*str)
		    l += font_putchar(font, *str++, dest ? &dest[l] : NULL, width);
	}
    return l;
}

int font_text(Font * font, char *str, uint16 * dest, int width)
{
	return font_text_truncate(font, str, dest, width, width);
}

/*
int font_text(Font *font, char *str, uint16 *dest, int width)
{
	int l = 0;

	while(*str)
		l += font_putchar(font, *str++, dest ? &dest[l] : NULL, width);

	return l;
}
*/

int font_text_multi(Font **fontlist, int *current, char *str, uint16 *dest, int width)
{
	Font *font;
	uint16 *outw = dest;

	font = fontlist[*current];

	while(*str)
	{
		if(*str <= 8)
			font = fontlist[*current = (*str++ - 1)];
		else
			dest += font_putchar(font, *str++, dest, width);
	}
	return dest-outw;
}

Font *font_dup(Font *font)
{
	Font *f = malloc(sizeof(Font));
	memcpy(f, font, sizeof(Font));
	return f;
}

Font *font_load(char *name)
{
	int size, pix_size, l;
	Font font;
	Font *rfont;
	uchar *mem;
	int fd = open(name, 0);
	if(fd > 0)
	{
		mem = (uchar *)lseek(fd, 0, SEEK_MEM);

		read(fd, &font, 8);
	
		pix_size = font.width * font.height;
		if(pix_size & 1) pix_size++;

		l = strlen(name);
		if(!(l & 1))
			l++;

		size = sizeof(Font);
		//size = sizeof(Font) + l + 1;

		rfont = malloc(size);
		memcpy(rfont, &font, 8);

		//fprintf(stderr, "FONT %p %d\n", rfont, size);

		//rfont->name = (char *)&rfont[1];
		//strcpy(rfont->name, name);

		mem += 8;
		
		if(rfont->flags & FFLG_COLOR)
		{
			rfont->colors = (uint16 *)&mem[2];
			l = *((uint16*)mem);
			mem += (l*2+2);
		}
		else
			rfont->colors = NULL;

		rfont->pixels = mem;

		if(rfont->flags & FFLG_PROPORTIONAL)
			rfont->offsets = (uint16 *)&mem[pix_size];
		else
			rfont->offsets = NULL;

		close(fd);

		return rfont;
	}
	return NULL;
}
