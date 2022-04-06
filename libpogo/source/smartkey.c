/*
 * Smartkey layer (smartkey.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * The smartkey system uses both the vkeyboard and the console and adds
 * functionality such as line-editing, history, dictionary shortcuts,
 * key sequences with assignable events etc and was initially made for
 * my ports of text-adventure interprators (Frotz, Magnetic, Level9)
 *
 **/

#include "core.h"
#include "device.h"
#include "font.h"
#include "editbuf.h"
#include "io.h"
#include "gba_defs.h"

#include "cursor.h"

#include "smartkey.h"

#ifdef DEBUG
#define DPRINTF _dprintf
#else
#define DPRINTF (void)
#endif

static int vkey_fd = 0;
static int con_fd = 1;
static uchar commit = 0;
static uchar linemode = 0;
static uchar current_mode = 0;
static uchar last_mode = 0;

static uchar con_mode;

typedef int (*LookupFunc)(int32, uint16);

typedef struct {
	uint32 seq;
	uint32 mask;
	LookupFunc func;
	uchar flags;
	uchar mode;
	uint16 cmd;
} KeyLookup;

static int lookup_count = 0;
static KeyLookup lookups[40];

static char smartbuf[256];

static Font *font;
static int fontheight = 8;
 
static Device smartdev;

#define HISTORY_SIZE 10
static char *hist_lines[HISTORY_SIZE];
static char histbuf[HISTORY_SIZE * 40];
static int hist_pos = 0;
static int hist_size = 0;

static char **dicts[4];

static int handle_marking(int32 k, uint16 c);

static int go_mode0(int32 k, uint16 c) { last_mode = current_mode; current_mode = 0; return 1; }
static int go_mode1(int32 k, uint16 c) { last_mode = current_mode; current_mode = 1; handle_marking(0,0); /* FROTZ HACK */ return 1; }
static int go_mode2(int32 k, uint16 c) { last_mode = current_mode; current_mode = 2; return 1; }
static int go_mode3(int32 k, uint16 c) { last_mode = current_mode; current_mode = 3; return 1; }

static int (*completion)(char *buf, char *res) = NULL;


static int handle_completion(int32 k, uint16 c)
{
	char tmp[40];
	if(completion)
	{
		int rc = completion(editbuf_get(), tmp);
		if(rc < 2)
			editbuf_write(tmp, strlen(tmp));
	}
	return 1;
}

static int handle_history(int32 k, uint16 c)
{
	switch(c)
	{
	case HISTORY_UP:
		if(hist_pos > 0) {
			editbuf_set(hist_lines[--hist_pos]);
		}
		break;
	case HISTORY_DOWN:
		if(hist_pos < (hist_size-1))
		{
			editbuf_set(hist_lines[++hist_pos]);
		} else
		if(hist_pos == (hist_size-1)) {
			editbuf_set("");
			hist_pos++;
		}

		break;
	case CURSOR_LEFT:
		editbuf_seek(-1, SEEK_CUR);
		break;
	case CURSOR_RIGHT:
		editbuf_seek(1, SEEK_CUR);
		break;
	}
	return 1;
}


static int handle_dict(int32 k, uint16 c)
{
	int m = 0;
	int sect = c - GET_DICT0;
	int i;

	if(sect < 0 || sect > 3 || !dicts[sect])
		return 0;

	if(k < 0x10)
		return 0;
	i = k & 0xF;
	if(i < 7 && i > 2)
	{
		m = (k>>4)&0xf;
		if(m < 7 && m > 2)
		{
			if(m < i)
				i = m<<4 | i;
			else
				i = i<<4 | m;
		}
	} else
		if(i !=9 && i != 10)
			return 0;
	m = 0;
	switch(i)
	{
	case 0x35: m = 1;
		break;
	case 0x03: m = 2;
		break;
	case 0x36: m = 3;
		break;
	case 0x05: m = 4;
		break;
	case 0x06: m = 5;
		break;
	case 0x45: m = 6;
		break;
	case 0x04: m = 7;
		break;
	case 0x46: m = 8;
		break;
	case 0x09: m = 9;
		break;
	case 0x0A: m = 10;
		break;
	}

	if(m && dicts[sect][m])
	{
		m--;
		editbuf_write(dicts[sect][m], strlen(dicts[sect][m]));
		return 1;
	}
	return 0;
}

static int show_keyb(int32 k, uint16 c)
{
	ioctl(vkey_fd, IO_SETMODE, KM_SHOWKBD);
	return 1;
}

static int cr_xpos = 0;
static int cr_ypos = 0;

#define is_word(x) ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') || (x >= '0' && x <= '9'))
static char *console_getchar(int col, int row)
{
	char *p = (char *)ioctl(con_fd, CC_GETLINE, row);
	int l = strlen(p);

	if(col > l) col = l;
	return &p[col];
}

static int handle_marking(int32 k, uint16 c)
{
	char tmp[128];
	int rc;
	char *p;
	switch(c)
	{
	case MARKER_UP:
		cr_ypos--;
		break;
	case MARKER_DOWN:
		cr_ypos++;
		break;
	case MARKER_WORDRIGHT:
		p = console_getchar(cr_xpos, cr_ypos);
		while(is_word(*p)) {
			p++;
			cr_xpos++;
			p = console_getchar(cr_xpos, cr_ypos);
		}
		while(*p && !is_word(*p)) {
			p++;
			cr_xpos++;
			p = console_getchar(cr_xpos, cr_ypos);
		}
		if(!*p) {
			cr_xpos = 0;
			cr_ypos++;
		}
		break;
	case MARKER_WORDLEFT:
		p = console_getchar(cr_xpos, cr_ypos);
		if(!p[-1]) {
			cr_xpos = 999;
			cr_ypos--;
		}
		while(p[-1] && !is_word(p[-1])) {
			p--;
			cr_xpos--;
			p = console_getchar(cr_xpos, cr_ypos);
		}
		while(is_word(p[-1])) {
			p--;
			cr_xpos--;
			p = console_getchar(cr_xpos, cr_ypos);
		}
		break;
/*	case MARKER_DICT:
	case MARKER_DICTBACK:
		handle_dict(k, c);*/
	case MARKER_INSERT:
	case MARKER_INSERTBACK:
		p = console_getchar(cr_xpos, cr_ypos);
		while(is_word(*p)) p--;
		p++;
		while(is_word(*p))
			editbuf_put(*p++);
		if(*p == ' ')
			editbuf_put(' ');
		break;
	}

	if(c == MARKER_DICTBACK || c == MARKER_INSERTBACK)
		current_mode = last_mode;

	if(cr_xpos < 0)
		cr_xpos = 0;
	if(cr_ypos < 1)
		cr_ypos = 1;
	if(cr_ypos > 18)
		cr_ypos = 18;

	strcpy(tmp, (char *)ioctl(con_fd, CC_GETLINE, cr_ypos));
	tmp[cr_xpos] = 0;
	rc = ioctl(con_fd, CC_GETWIDTH, tmp);
	cursor_set(rc, cr_ypos * fontheight );
	return 1;
}


const LookupFunc func_list[] = { go_mode0, go_mode1, go_mode2, go_mode3,
handle_history, handle_history, handle_history, handle_history, handle_dict, 
show_keyb, handle_completion, handle_marking, handle_marking, handle_marking, handle_marking, handle_marking,
handle_marking, handle_marking, handle_marking, handle_dict, handle_dict, handle_dict, handle_dict
};

static int insertchar(int32 k, uint16 cmd)
{
	char tmp[32];
	int rc;
	switch(cmd) {
	case 10:
		editbuf_seek(0, SEEK_END);
		editbuf_put(10);
		editbuf_commit();
		commit = 1;
		break;
	case 8:
		editbuf_del(-1, 1);
		break;
	case 9:
		if(completion)
		{
			rc = completion(editbuf_get(), tmp);
			if(rc < 2)
				editbuf_write(tmp, strlen(tmp));
		}
		break;
	default:
		editbuf_put(cmd);
		break;
	}
	return 1;
}


static int key_bindmode(char *pattern, int cmd, int mode)
{
	int i = 0;
	int flags = 0;
	uint32 mask = 0xFFFFFFFF;
	uint32 seq = 0;
	char *p = pattern;
	while(*p)
	{
		seq = seq<<4;
		i++;
		switch(*p)
		{
		case 'U':
			seq |= RAWKEY_UP;
			break;
		case 'D':
			seq |= RAWKEY_DOWN;
			break;
		case '<':
			seq |= RAWKEY_LEFT;
			break;
		case '>':
			seq |= RAWKEY_RIGHT;
			break;
		case 'L':
			seq |= RAWKEY_L;
			break;
		case 'R':
			seq |= RAWKEY_R;
			break;
		case 'S':
			seq |= RAWKEY_START;
			break;
		case 'E':
			seq |= RAWKEY_SELECT;
			break;
		case 'A':
			seq |= RAWKEY_A;
			break;
		case 'B':
			seq |= RAWKEY_B;
			break;
		default:
			i--;
			seq = seq>>4;
			switch(*p)
			{
			case '(':
				//any_start = p+1;
				break;
			case ')':
				break;
			case '!':
				flags |= 1;
				break;
			case '+':
				flags |= 2;
				break;
			case '*':
				mask = 0xFFFFFFFF << (32-i*4);
				break;
			default:
				return 0;
			}
		}
		p++;
	}
	for(i=0; i<8 && !((seq>>28)&0xf); i++, seq = seq << 4);
	i = lookup_count++;
	//while(((seq>>28)&0xf) == 0) seq = (seq<<4);
	lookups[i].seq = seq;
	lookups[i].mask = mask;
	lookups[i].flags = flags;
	if(cmd < 256)
		lookups[i].func = insertchar;
	else
		lookups[i].func = func_list[cmd-256];
	lookups[i].mode = mode;
	lookups[i].cmd = cmd;
	return 1;
}

static int smart_ioctl(int fd, int req, va_list vl)
{
	int i;
	char *s;
	switch(req) {
	case KC_ADDHISTORY:
		if(hist_size  == HISTORY_SIZE) {
			s = hist_lines[0];
			for(i=0; i<hist_size-1; i++) {
				hist_lines[i] = hist_lines[i+1];
			}
			hist_lines[i] = s;
			strcpy(hist_lines[hist_size-1], va_arg(vl, char*));
		} else {
			strcpy(hist_lines[hist_size++], va_arg(vl, char*));
		}
		hist_pos = hist_size;
		return hist_pos;
	case KC_SETDICT:
		i = va_arg(vl, int);
		if(i >= 0 && i <=3)
			dicts[i] = va_arg(vl, char**);
		return 0;
	case KC_BINDKEY:
		key_bindmode(va_arg(vl, char *), va_arg(vl, int), va_arg(vl, int));
		return 0;
	case KC_SETCOMPLETION:
		completion = va_arg(vl, void*);
		return 0;
	case IO_GETMODE:
		return con_mode;
	case IO_SETMODE:
		con_mode = va_arg(vl, int);
		cursor_show(con_mode & CM_SHOWCURSOR);
		return 0;
//	case KC_SETDICT:
//		dict[va_arg(vl, int)][va_arg(vl,int)] = va_arg(vl, char *);
//		return 0;
	}
	return -1;
}


void smart_linemode(int l)
{
	linemode = l;
}


static uint32 index2mask(int index)
{
	if(index < 3)
		return index;
	return 1<<(index-1);
}

static int handle_sequence(uint32 seq, int commit)
{
	int i;
	KeyLookup *lm;
	int matches = 0;
	int seq2 = seq;

	for(i=0; i<8 && !((seq>>28)&0xf); i++, seq = seq << 4);

	for(i=0; i<lookup_count; i++)
		if(lookups[i].seq == (lookups[i].mask & seq) && (lookups[i].mode == current_mode || (!linemode && (lookups[i].mode == 4))))
		{
			lm = &lookups[i];
			if( (commit || (lm->flags & 1)))
			{
				if(lm->func(seq2, lm->cmd))
					return 1;
			}

			matches++;
		}
	return 0;
}

static uint32 pressed = 0;
static uint32 seq = 0;
static uint32 last_released = 0;

static void handle_normal(void)
{
	int rc;
	char c;
	char tmp[32];
	//update_cursor();
	if(read(vkey_fd, &c, 1) > 0) {
		switch(c)
		{
		case 10:
			editbuf_seek(0, SEEK_END);
			editbuf_put(10);
			editbuf_commit();
			commit = 1;
			break;
		case 8:
			editbuf_del(-1, 1);
			break;
		case 9:
			if(completion)
			{
				rc = completion(editbuf_get(), tmp);
				if(rc < 2)
					editbuf_write(tmp, strlen(tmp));
			}
			break;
		default:
			editbuf_put(c);
		}
	}
}

static int seq_commited = 0;
static int handle_smart(void)
{
	int i;
	uint32 newseq,l;
	char c;

	ioctl(vkey_fd, IO_SETMODE, KM_RAW);
	c = 0x11;
	/* Read keybuffer */
	while(read(vkey_fd, &c, 1) == 1)
	{

		/* If a key was pressed */
		if(!(c & 0x80))
		{
			if(last_released)
			{
				newseq = 0;
				/* A key was released then a new key pressed - validate whole sequence */
				for(i=0; i<8; i++)
				{
					/* Any keys pressed last time will be kept in the sequence.
					   (The new key will be added after this )*/
					//DPRINTF("comparing with %x %x %x\n", seq, seq>>28, index2mask(seq>>28)); 
					if(pressed & (l = index2mask(seq>>28)))
					{
						newseq = newseq << 4;
						newseq |= (seq>>28);
					}
					seq = seq<<4;
				}
				seq = newseq;
				seq_commited = 0;
			}
			/* Add key to sequence */
			seq = seq<<4;
			seq |= c;

			/* Look for a sequence handler */
			if(handle_sequence(seq, 0))
				seq_commited = 1;

			last_released = 0;

			/* Update pressed keys */
			pressed |= index2mask(c);

		}
		else
		{
			/* A key was released */
			c &= 0x7F;
			pressed &= ~(index2mask(c));
			last_released = c;
			
			if(!pressed)
			{
				/* All keys released, handle sequence */
				if(!seq_commited)
					handle_sequence(seq, 1);
				last_released = 0;
				seq = 0;
				seq_commited = 0;
			}
		}
//		DPRINTF("Read %02x => seq = %x, pressed = %x\n", c, seq, pressed);
	}

	/* Hack to avoid keys sticking */
/*	i = GETW(REG_KEYINPUT);
	i = (~i) & 0x3FF;
	if(!i)
	{
		pressed = 0;
		seq = 0;
		seq_commited = 0;
	}*/
	return 0;
}

static int maxl = 0;

static void update_line(void)
{
	char c = 10;
	int rc,l,sl;
	const char spaces[] = "                                                    ";
	char *s = editbuf_get();
	int cl = font->charwidth;
	l = ioctl(con_fd, CC_GETWIDTH, s);
	if(l > maxl)
		maxl = l;

	sl = strlen(s);
	rc = ioctl(con_fd, CC_GETXY);
	if(sl && s[sl-1] == 10) {
		write(con_fd, s, sl-1);
		write(con_fd, spaces, (maxl-l+cl-1)/cl);
		write(con_fd, &c, 1);
		//ioctl(con_fd, CC_GOTOXY, 0, (rc&0xffff) + 1);
		printf("\033[%d;%dH", (rc&0xffff) + 1, 0);
		//DPRINTF("ULINE %d %d '%s' %d\n",*s, s[sl-1], s, sl);
	} else {
		write(con_fd, s, sl);
		write(con_fd, spaces, (maxl-l+cl-1)/cl);
		//DPRINTF("%s is %d pixels, max %d (%d)\n", s, l, maxl, cl);
		//ioctl(con_fd, CC_GOTOXY, rc>>16, rc&0xffff);
		printf("\033[%d;%dH", rc&0xffff, rc>>16);
	}
}

int smart_read(int fd, char *s, int count)
{
	int c,l;
	char tmp[80];
	int mode = ioctl(vkey_fd, IO_GETMODE);

	cursor_show(con_mode & CM_SHOWCURSOR);

	/* Read characters to sbuf depending on mode */
	if(mode & KM_SHOWKBD)
		handle_normal();
	else
		handle_smart();

	strcpy(tmp, editbuf_get());
	if(linemode && editbuf_changed() && !commit)
	{
		update_line();
	}

	strcpy(tmp, editbuf_get());
	tmp[editbuf_seek(0, SEEK_CUR)] = 0;
	l = ioctl(con_fd, CC_GETWIDTH, tmp);
	c = ioctl(con_fd, CC_GETXY)>>16;
	//DPRINTF("%d+%d\n", c, l);
	if(current_mode == 0)
		cursor_set(c+l, (ioctl(con_fd, CC_GETXY)&0xFFFF) * fontheight );

	if(!linemode)
		editbuf_commit();

	if(commit)
		maxl = 0;

	commit = 0;
	return editbuf_read(s, count);

}

static int smart_open(const char *name, int mode)
{
	return 0;
}

void smartkey_init(void)
{
	int i;
	vkey_fd = 0; //open("/dev/stdin", 0);
	con_fd = 1; //open("/dev/stdout", 0);

	font = (Font *)ioctl(con_fd, CC_GETFONT, 0);
	if(font && font != (Font *)-1)
	{
		fontheight = font->height;
//		DPRINTF("Font: %d width, %d height, %d charwidth\n", font->width, font->height, font->charwidth);
	}

	memset(&smartdev, 0, sizeof(smartdev));
	smartdev.open = smart_open;
	smartdev.ioctl = smart_ioctl;
	smartdev.read = (int (*)(int, void *, int))smart_read;

	editbuf_init(smartbuf, sizeof(smartbuf));

	for(i=0; i<HISTORY_SIZE; i++)
		hist_lines[i] = &histbuf[i*30];

	cursor_init();
	seq = 0;
	for(i=0; i<4; i++)
		dicts[i] = NULL;
	device_register(&smartdev, "/dev/smartkey", NULL, -1);
}

