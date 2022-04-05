/*
 * Text console device (console.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Device for text output. Supports fixed and proportional fonts,
 * limited ANSI (colors, positioning, bold, scrolling etc).
 *
 **/

#include "core.h"
#include "font.h"
#include "device.h"
#include "screen.h"
#include "gba_defs.h"

#include "console.h"

#define TO_RGB16(r,g,b) ( ((b << 7) & 0x7C00) | ((g << 2) & 0x03E0) | (r >> 3) )

const static uint16 ansi_colors[16*3] =
{ 
	TO_RGB16(0x00,0x00,0x00),
	TO_RGB16(0xC0,0x00,0x00),
	TO_RGB16(0x00,0xC0,0x00),
	TO_RGB16(0xC0,0xC0,0x00),
	TO_RGB16(0x00,0x00,0xC0),
	TO_RGB16(0xC0,0x00,0xC0),
	TO_RGB16(0x00,0xC0,0xC0),
	TO_RGB16(0xC0,0xC0,0xC0),

	TO_RGB16(0x00,0x00,0x00),
	TO_RGB16(0xFF,0x00,0x00),
	TO_RGB16(0x00,0xFF,0x00),
	TO_RGB16(0xFF,0xFF,0x00),
	TO_RGB16(0x00,0x00,0xFF),
	TO_RGB16(0xFF,0x00,0xFF),
	TO_RGB16(0x00,0xFF,0xFF),
	TO_RGB16(0xFF,0xFF,0xFF),
};

typedef struct {
	Font *font;
	uint16 *pixels;
	uint16 width;
	uint16 height;
	int bufsize;
	int mode;
	int textpos;
	int x,y;
	int rborder;
	int bborder;
} Console;

static Font *fonts[4];

static int attrs;// = 0;

static uint16 fgcol;// = 0;
static uint16 bgcol; // = 0;

#define MAX_LINES 30
#define MAX_COLS 60

static char lines[MAX_LINES][MAX_COLS];

/* Points to one string for each line in buffer/screen */
static char *lineptrs[MAX_LINES];

/* Points to current char in current buffer */
char *lineptr;

static Console default_console;
static Console *con;


static Device condev;

static char *console_getline(int l)
{
	return lineptrs[l];
}

int scroll_start = 0;
int scroll_stop = 20;

static void console_scroll(int start, int rows, int count)
{
	int size;
	char *str;
	uint16 col = bgcol; // | (bgcol<<8);

	int i,fh = con->font->height;
	uint16 *d = (uint16 *)&con->pixels[start*fh*con->width];
	uint16 *s = (uint16 *)&con->pixels[(start+1)*fh*con->width];

	size = ((rows-1) * fh * con->width);

	while(size--)
		*d++ = *s++;
	size = (fh * con->width);
	while(size--)
		*d++ = col;

	str = lineptrs[start];
	for(i=start; i<(start+rows)-1; i++) {
		if(lineptr == lineptrs[i+1])
			lineptr = lineptrs[i];
		lineptrs[i] = lineptrs[i+1];
	}
	if(lineptr == lineptrs[(start+rows)-1])
		lineptr = str;
	lineptrs[(start+rows)-1] = str;
	*str = 0;
}

static void scroll_screen(void)
{
	console_scroll(scroll_start, scroll_stop-scroll_start, 1);
}

static void clear_screen(void)
{
	uint32 *p = (uint32*)con->pixels;
	uint32 col = bgcol | (bgcol<<16);
	int i = 120*160;
	while(i--)
		*p++ = col;

	for(i=0; i<MAX_LINES; i++) {
		lineptrs[i] = lines[i];
		lines[i][0] = 0;
	}

}

static void console_realxy(int x, int y)
{
	int w = 0;

	lineptr = lineptrs[y];

	while(x > w) {
		if(!(*lineptr)) { 
			*lineptr = ' ';
			lineptr[1] = 0;
		}
		w += font_putchar(con->font, *lineptr, NULL, con->width);
		lineptr++;
	}
	//x += (w-x);

	con->x = x;
	con->y = y;
}

/*
static void console_gotoxy(int x, int y)
{
	int w = 0;

	lineptr = lineptrs[y];

	while(x) {
		if(!(*lineptr)) {
			*lineptr = ' ';
			lineptr[1] = 0;
		}
		w += font_putchar(con->font, *lineptr, NULL, con->width);
		lineptr++;
		x--;
	}
	con->x = w;
	con->y = y;
}
*/

static void console_areaclear(int start, int rows)
{
	int l,i;
	uint16 col = bgcol; //| (bgcol<<8);
	int rowsize = con->font->height * con->width;
	uint16 *ptr = (uint16 *)&con->pixels[start * rowsize];

	for(i=start; i<(start+rows); i++) {
		l = rowsize;
		while(l--)
			*ptr++ = col;

		lineptrs[i][0] = 0;
	}
}

static void console_linekill(void)
{
	console_areaclear(con->y, 1);
	con->x = 0;
	lineptr = lineptrs[con->y];
	*lineptr = 0;
}

static void console_setcolor(int fg, int bg)
{
	fgcol = fg;
	bgcol = bg;
	font_setcolor(ansi_colors[fg], ansi_colors[bg]);
}

#define isalpha(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define isdigit(c) ((c >= '0' && c <= '9') || (c == '-'))

static int atoi(const char *s)
{
	int v = 0;
	uchar neg = 0;

	while(*s == ' ')
		*s++;

	if(*s == '-') {
		s++;
		neg = 1;
	}

	while(isdigit(*s)) {
		v *= 10;
		v += (*s - 0x30);
		s++;
	}
	if(neg)
		v = -v;

	return v;
}

int parse_ansi(char *s, int *args)
{
	int i=0;
	if(*s == '[') s++;

	while(!isalpha(*s))
	{
		if(isdigit(*s))
		{
			args[i] = atoi(s);
			while(isdigit(*s))
				s++;
			continue;
		}
		if(*s == ';')
			i++;
		s++;
	}
	return i+1;

}

void handle_esc(char *s, char c)
{
	int args[4] = {0, 0, 0, 0};
	int w,i,l;
	uint16 *ptr = &con->pixels[con->x + (con->y * con->font->height) * con->width];

	switch(c)
	{
	case '(':
		con->font = fonts[0];
		break;
	case ')':
		con->font = fonts[1];
		break;
	case 'f':
		parse_ansi(s, args);
		con->font = fonts[args[0]];
		break;
	case 'K':
		w = con->x;
		while((w + con->font->charwidth) < con->width) {
			w += font_putchar(con->font, ' ', ptr, con->width);
			ptr += con->font->charwidth;
		}
		*lineptr = 0;
		break;
	case 'H':
		l = parse_ansi(s, args);
		console_realxy(args[1], args[0]);
		break;
	case 'A':
		args[0] = 1;
		parse_ansi(s, args);
		con->y -= args[0];
		break;
	case 'B':
		args[0] = 1;
		parse_ansi(s, args);
		con->y += args[0];
		break;
	case 'r':
		args[0] = 0;
		args[1] = con->bborder;
		parse_ansi(s, args);
		scroll_start = args[0];
		scroll_stop = args[1];
		break;
	case 'M':
		console_scroll(scroll_start, scroll_stop, 1);
		break;
	case 'm':
		l = parse_ansi(s, args);
		for(i=0; i<l; i++) {
			if(args[i] < 9)
				attrs = args[i];
			else
			if(args[i] < 38)
				fgcol = args[i] - 30;
			else
				bgcol = args[i] - 40;
		}

/*
		if(attrs == 7)
			font_setcolor(bgcol, fgcol + (attrs&2 ? 0 : 8));
		else*/
		//if(fgcol > 8)
		//	fgcol -= 8;
		if(attrs & 2)
			con->font->flags |= FFLG_BOLD;
		else
			con->font->flags &= (~FFLG_BOLD);

		//font_setcolor((attrs & 4) ? fgcol + 8 : fgcol, bgcol);
		font_setcolor(ansi_colors[fgcol], ansi_colors[bgcol]);
	}
}

static uchar esc_mode = 0;
static char escstring[10];
static char *escptr;

static void console_putchar(char c)
{
	char *p;
	int w = 0;

	Font *font = con->font;
	if(attrs & 4)
		font = fonts[3];

	//if(pos >= bufsize)
	//	scroll(con);
	uint16 *ptr = &con->pixels[con->x + (con->y * font->height) * con->width];

	if(esc_mode) {
		*escptr++ = c;
		*escptr = 0;
		esc_mode--;
		if(isalpha(c)) {
			esc_mode = 0;
			handle_esc(escstring, c);
		}
		return;
	}

	switch(c) {
	case CHAR_LEFT:
		break;
	case CHAR_RIGHT:
		break;
	case 8:
		p = lineptrs[con->y];
		if(lineptr > p) {
			w = font_putchar(font, *(--lineptr), NULL, con->width);
			*lineptr = 0;
			ptr -= w;
			font_text(font, "  ", ptr, con->width);
			w = -w;
		}
		break;
	case 12:
		w = 0;
		con->x = con->y = 0;
		clear_screen();
		break;
	case 27:
		esc_mode = 10;
		escptr = escstring;
		*escptr = 0;
		return;
	case 10:
	case 13:
		w = 0;
		con->x = 0;

		if((con->y + 1) >= con->bborder)
		{
			if(con->mode & CM_LINEWRAP)
				scroll_screen();
		}
		else
			con->y++;
		lineptr = lineptrs[con->y];
		*lineptr = 0;
		break;
	case 9:
		//w = font_text(font, "    ", NULL, con->width);
		w = font_text(font, "    ", ptr, con->width);
		break;
	case 250:
		console_linekill();
		w = 0;
		break;
	default:
		//if(c < font->first || c > font->last)
		//	c = ' ';
		w = font_putchar(font, c, NULL, con->width);
		if(w + con->x > con->rborder)
		{
			if(con->mode & CM_LINEWRAP)
			{
				if((con->y + 1) >= con->bborder)
					scroll_screen();
				else
					con->y++;
				con->x = 0;
				lineptr = lineptrs[con->y];
				*lineptr = 0;
				ptr = &con->pixels[con->x + (con->y * font->height) * con->width];
			}
			else
				return;
		}
		if(w)
			*lineptr++ = c;
		*lineptr = 0;
		w = font_putchar(font, c, ptr, con->width);
	}
	con->x += w;
}

static void console_setfont(Font *font, int num)
{
	if(font)
	{
		fonts[num] = font;
		con->font = font;
	} else
		con->font = fonts[num];
	con->bborder = con->height / font->height;
}

static int console_write(int fd, const void *dest, int size)
{
	int i;
	char *s = (char *)dest;
	for(i=0; i<size; i++)
		console_putchar(s[i]);

	return size;
}

static int console_read(int fd, void *dest, int size)
{
	return 0;
}

static int console_lseek(int fd, int offset, int origin)
{
	return -1;
}

void console_refresh(void)
{
	uint32 *p = (uint32*)con->pixels;
	uint16 *ptr = con->pixels;
	uint32 col = bgcol | (bgcol<<8) | (bgcol<<16) | (bgcol<<24);
	int i = 60*160;
	while(i--)
		*p++ = col;

	ptr = con->pixels;
	for(i=0; i<con->bborder; i++)
	{
		font_text(con->font, lineptrs[i], ptr, con->width);
		ptr += (con->font->height * con->width);
	}
}
						 

static int console_open(const char *name, int mode)
{
	return 0;
}

static int console_ioctl(int fd, int req, va_list vl)
{
	char *s;

	switch(req) {
	case CC_CLEAR:
		console_areaclear(va_arg(vl, int), va_arg(vl, int));
		return 0;
	case IO_GETPARAM:
		switch(va_arg(vl, int))
		{
			case CP_TEXWIDTH:
				s = va_arg(vl, char*);
				return font_text(con->font, s, NULL, con->width);
			case CP_WIDTH:
				return con->width;
			case CP_HEIGHT:
				return con->height;
			default:
				return -1;
		}
	case CC_GETWIDTH:
		s = va_arg(vl, char*);
		return font_text(con->font, s, NULL, con->width);
	case IO_SETMODE:
		con->mode = va_arg(vl, int);
		return 0;
	case IO_GETMODE:
		return con->mode;
	case CC_SETFONT:
		console_setfont(va_arg(vl, Font*), va_arg(vl, int));
		return 0;
	case CC_GETFONT:
		return (int)fonts[va_arg(vl, int)];
	case CC_GETXY:
		return (con->x << 16) | con->y;
	case CC_GETLINE:
		return (int)console_getline(va_arg(vl, int));
	}
	return -1;
}

void console_init(void)
{
	uint16 *pixels;
	int width, height, fd;

	con = &default_console;

	fd = open("/dev/screen", 0);

	if(fd >= 0)
	{
		ioctl(fd, SC_SETMODE, 2);
		pixels = (uint16 *)lseek(fd, 0, SEEK_MEM);
		width = ioctl(fd, IO_GETPARAM, SCREEN_WIDTH);
		height = ioctl(fd, IO_GETPARAM, SCREEN_HEIGHT);
		close(fd);
	} else
		return;

	con->font = NULL;
	con->pixels = pixels;
	con->x = 0;
	con->y = 0;
	con->textpos = 0;
	con->width = width;
	con->height = height;
	con->rborder = width;
	con->bborder = 20;
	con->mode = 0;

	clear_screen();
	console_setcolor(15, 0);
	fgcol = 7;

	lineptr = lineptrs[0];

	memset(&condev, 0, sizeof(condev));
	condev.open = console_open;
	condev.write = console_write;
	condev.ioctl = console_ioctl;
	condev.read = console_read;
	condev.lseek = console_lseek;

	device_register(&condev, "/dev/con", NULL, 1);
}
