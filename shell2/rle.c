/*
 * PooShell sourcecode (rle.c) - created by Jonas Minnberg 2002
 *
 * RLE packing and unpacking functions.
 *
 ****/

#include <pogo.h>

extern char *clipboard;

void set_ram_start(int i);

void rle_unpack(char *src, char *dest, int size)
{
	int count, rep_char;
	char *dst = dest;
	char *s = src+size;

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

int rle_pack(char *src, char *dest, int size)
{
	int count, rep_char;
	char *cpos;
	char *dst = dest;
	char *s = src + size;

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
	char *ptr = (char *)0x0E00FFFF;

	char *buf = clipboard;

	set_ram_start(bank);
	while((ptr >= (char *)0x0E000000) && (ptr[0] == 0)) ptr--;
	count = (int)ptr - 0x0E000000 + 1;

	if(!count)
		return 0;

	count = rle_pack((char *)0x0E000000, buf, count);

	if(write(fd, buf, count) < 0)
		count = -1;

	//close(fd);

	return count;
}

int load_rle(int fd, int bank)
{
	int rc;
	char *buf = clipboard;
	if(fd >= 0)
	{
		rc = read(fd, buf, 65536);
		set_ram_start(bank);

		rle_unpack(buf, (char *)0x0E000000, rc);
		return rc;
	}
	return 0;
}
