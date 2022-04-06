#include "core.h"
#include "string.h"
#include "sram_access.h"

void set_ram_start(int i);

char srambuf[1024];

static char * const sram_mem = (char *)0x0E000000;

void sram_read_bytes(const char *sram_src, int size, void *dest)
{
	char *d;
	int s;
	int bank;

	if(sram_src < sram_mem)
	{
		if(size)
			return (void)memcpy8(dest, sram_src, size);
		strcpy(dest, sram_src);
		return;
	}

	d = (char *)dest;
	s = (int)sram_src & 0xFFFF;
	bank = ((((int)sram_src)>>16) & 0xFF);
	set_ram_start(bank);

	/*{
		char tmp[60];
		sprintf(tmp, "bank = %d, src = %x (%p)\n", bank, s, sram_src);
		dprint(tmp);
	}*/

	if(size)
	{
		while(size--)
		{
			*d++ = sram_mem[s++];
			if(!(s & 0xFFFF))
			{
				s = 0;
				set_ram_start(++bank);
			}
		}
	}
	else
	{
		while(sram_mem[s])
		{
			*d++ = sram_mem[s++];
			if(!(s & 0xFFFF))
			{
				s = 0;
				set_ram_start(++bank);
			}
		}
		*d = 0;
	}
}

void sram_write_bytes(void *sram_dest, int size, void *src)
{
	char *s;
	int d; 
	int bank;

	if((char *) sram_dest < sram_mem)
	{
		if(size)
			return (void)memcpy8(sram_dest, src, size);
		strcpy(sram_dest, src);
		return;
	}

	d = (int)sram_dest & 0xFFFF;
	bank = ((((int)sram_dest)>>16) & 0xFF);
	s = (char *)src;
	set_ram_start(bank);

	/*{
		char tmp[60];
		sprintf(tmp, "bank = %d, dst = %x (%p)\n", bank, d, sram_dest);
		dprint(tmp);
	}*/


	if(size)
	{
		while(size--)
		{
			sram_mem[d++] = *s++;
			if(!(d & 0xFFFF))
			{
				d = 0;
				set_ram_start(++bank);
			}
		}
	}
	else
	{
		while(*s)
		{
			sram_mem[d++] = *s++;
			if(!(d & 0xFFFF))
			{
				d = 0;
				set_ram_start(++bank);
			}
		}
		sram_mem[d] = 0;
	}
}


void sram_strcpy(char *dest, const char *src)
{
	/*dprint("STRCPY\n");
	{
		char tmp[16];
		sprintf(tmp, "SRAM %p\n", srambuf);
		dprint(tmp);
	}*/
	sram_read_bytes((void *) src, 0, srambuf);
	sram_write_bytes(dest, 0, srambuf);
}

int sram_strcmp(const char *a, const char *b)
{
	int l;

	sram_read_bytes((void *) b, 0, &srambuf[512]);
	l = strlen(&srambuf[512]);
	if (l < 512)
		sram_read_bytes((void *) a, l+1, srambuf);
	else
		return -1;

	return strcmp(srambuf, &srambuf[512]);

}

void sram_memset(char *dest, int val, int len)
{
	int l;
	memset(srambuf, val, 1024);

	while(len > 0)
	{
		l = (len < 1024 ? len : 1024);
		sram_write_bytes(dest, l, srambuf);
		len -= l;
		dest += l;
	}
}

void sram_memmove(void *d, void *s, int len)
{
	char *src = s, *dest = d;
	int l;

	if(dest > src)
	{
		dest += len;
		src += len;
		while(len > 0)
		{
			l = (len < 1024 ? len : 1024);
			sram_read_bytes(src-l, l, srambuf);
			sram_write_bytes(dest-l, l, srambuf);
			len -= l;
			src -= l;
			dest -=l;
		}

	}
	else
	{
		while(len > 0)
		{
			l = (len < 1024 ? len : 1024);
			sram_read_bytes(src, l, srambuf);
			sram_write_bytes(dest, l, srambuf);
			len -= l;
			src += l;
			dest += l;
		}
	}
}

void sram_memcpy(void *d, void *s, int len)
{
	int l;
	char *dest = d, *src = s;

	/*dprint("MEMCPY\n");
	{
		char tmp[16];
		sprintf(tmp, "SRAM %p\n", srambuf);
		dprint(tmp);
	}*/
	while(len > 0)
	{
		l = ((len < 1024) ? len : 1024);
		sram_read_bytes(src, l, srambuf);
		sram_write_bytes(dest, l, srambuf);
		len -= l;
		src += l;
		dest += l;
	}
}
