/*
 * Editbuffer for virtual keyboard (editbuf.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * A device-like implementation of an editbuffer used for line-editing.
 *
 **/

#ifdef WIN32
#include <stdio.h>
#else
#include "core.h"
#include "device.h"
#endif

/*

  The editbuffer has a reading-part and an edit-part. The edit_start marker
  marks the beginning of the edit-part. You can not read beyond this point.

  buf_start             edit_start    edit_pos        edit_end
  |                    A|             |               |0


  Bytes are always read from buf_start, and everything is copied backwards
  after reading.

  When just adding, edit_pos == edit_end

  eg we read n bytes so we copy (edit_end-buf_start-n) bytes from buf_start+n to buf_start
  and decrease all edit_ pointers by n


*/

char *editbuf;
int editsize;
char *edit_start;
char *edit_pos;
char *edit_end;
int changed = 0;

int editbuf_read(char *dest, int count)
{

	int n = count;
	char *s = editbuf;

	/* Copy from buffer to dest, up to count values */
	while((s < edit_start) && count) {
		*dest++ = *s++;
		count--;
	}

	n -= count;

	/* Copy unread chars to beginning of buffer */
	dest = editbuf;
	while(s <= edit_end) {
		*dest++ = *s++;
	}

	edit_start -= n;
	edit_pos -= n;
	edit_end -= n;

	return n;
}

void editbuf_init(char *mem, int size)
{
	editbuf = edit_start = edit_pos = edit_end = mem;
	editsize = size;
	*editbuf = 0;
	changed = 0;
}

void editbuf_put(int c)
{
	memmove(edit_pos+1, edit_pos, (edit_end - edit_pos + 1));
	*edit_pos = c;
	edit_pos++;
	edit_end++;
	*edit_end = 0;
	changed = 1;
}

void editbuf_write(char *s, int count)
{
	memmove(edit_pos+count, edit_pos, edit_end-edit_pos);
	memcpy(edit_pos, s, count);
	edit_pos += count;
	edit_end += count;
	*edit_end = 0;
	changed = count;
}

char *editbuf_get(void)
{
	return edit_start;
}

int editbuf_changed(void)
{
	int rc = changed;
	changed = 0;
	return rc;
}

void editbuf_set(char *s)
{
	strcpy(edit_start, s);
	edit_end = edit_pos = &edit_start[strlen(s)];
	*edit_pos = 0;
	changed = 1;
}

int editbuf_del(int start, int count)
{
	int len;
	char *s;
	char *e;

	if(start == -1)
		start = edit_pos-edit_start-1;
	if(start < 0) {
		count += start;
		start = 0;
	}
	if(count <= 0)
		return 0;

	changed = 1;

	s = &edit_start[start];
	e = &edit_start[start+count];

	if(e > (edit_end+1))
		e = (edit_end+1);

	len = ((edit_end+1) - e);

	memcpy(s, e, len);
	edit_end -= count;

	if(edit_pos > s) {
		edit_pos -= count;
		if(edit_pos < s)
			edit_pos = s;
	}
	return 0;
}

int editbuf_commit(void)
{
	edit_start = edit_pos = edit_end;
	*edit_start = 0;
	return 0;
}

int editbuf_seek(int offset, int where)
{
	switch(where) {
	case SEEK_SET:
		edit_pos = &edit_start[offset];
		break;
	case SEEK_CUR:
		edit_pos = &edit_pos[offset];
		break;
	case SEEK_END:
		edit_pos = &edit_end[offset];
		break;
	}
	if(edit_pos < edit_start)
		edit_pos = edit_start;
	if(edit_pos > edit_end)
		edit_pos = edit_end;

	return edit_pos - edit_start;
}
