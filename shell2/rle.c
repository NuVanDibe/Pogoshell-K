/*
 * PooShell sourcecode (rle.c) - created by Jonas Minnberg 2002
 *
 * RLE packing and unpacking functions.
 *
 ****/

#include <pogo.h>
#include "freespace.h"

void set_ram_start(int i);

void rle_unpack(uchar *src, uchar *dest, int size)
{
	int count, rep_char;
	uchar *dst = dest;
	uchar *s = src+size;

	while(src < s)
	{
		count = *src++;
		//_dprintf("Copying %d bytes\n", count);
		while(count--)
			*dst++ = *src++;
		count = *src++;
		rep_char = *src++;
		//_dprintf("Repating %c %d times\n", rep_char, count);
		while(count--)
			*dst++ = rep_char;
	}
}

int rle_pack(uchar *src, uchar *dest, int size)
{
	int count, rep_char;
	uchar *cpos;
	uchar *dst = dest;
	uchar *s = src + size;

	while(src < s)
	{
		count = 0;
		cpos = dst++;
		while((count < 255) && (src[0] != src[1]) && (src < s))
		{
			*dst++ = *src++;
			count++;
		}
		*cpos = count;
		rep_char = src[0];
		count = 0;

		while((count < 255) && (*src == rep_char) && (src < s))
		{
			src++;
			count++;
		}

		*dst++ = count;
		*dst++ = rep_char;
	}
	return dst-dest;
}

int save_rle(int fd, int bank)
{
	int count;
	uchar *ptr = (uchar *)0x0E00FFFF;

	uchar *buf = (uchar *)(FREEPTR);

	set_ram_start(bank);
	while((ptr >= (uchar *)0x0E000000) && (ptr[0] == 0)) ptr--;
	count = (int)ptr - 0x0E000000 + 1;

	if(!count)
		return 0;

	count = rle_pack((uchar *)0x0E000000, buf, count);

	if(write(fd, buf, count) < 0)
		count = -1;

	//close(fd);

	return count;
}

int load_rle(int fd, int bank)
{
	int rc;
	uchar *buf = (uchar *)(FREEPTR);
	if(fd >= 0)
	{
		rc = read(fd, buf, 65536);
		set_ram_start(bank);

		rle_unpack(buf, (uchar *)0x0E000000, rc);
		return rc;
	}
	return 0;
}
