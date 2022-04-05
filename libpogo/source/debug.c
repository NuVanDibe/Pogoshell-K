/*
 * Debug functions (debug.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * stderr device for text output to debug console
 *
 **/

#include "core.h"
#include "device.h"
#include "font.h"

static Device debdev;

extern void dprint(const char *sz);

/*extern void __PrintStr (char *str);

void dprint2(const char *sz)
{
	__PrintStr(sz);
}
*/

static int deb_open(const char *name, int mode)
{
	return 0;
}

static int deb_write(int fd, const void *dest, int size)
{
	char tmp[128];
	memcpy(tmp, dest, size);
	tmp[size] = 0;
	dprint(tmp);
	return size;
}

void deb_init(void)
{
	memset(&debdev, 0, sizeof(debdev));
	debdev.open = deb_open;
	debdev.write = deb_write;
	device_register(&debdev, "/dev/deb", NULL, 2);
}
