/*
 * Misc. functions (misc.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Some general c-library functions
 *
 **/

#include "core.h"
#include "device.h"

void exit(int rc)
{
	while(1);
}

int atexit(void *func)
{
	return 0;
}

void abort(void)
{
	while(1);
}

static int seed = 11177;

int rand(void)
{
	/* Completely bogus random numbers, to be fixed */
	seed = (seed * 119) ^ 19117;
	return (seed * 19517) + 1767;

}

int getseed(void)
{
	return seed;
}

void srand(unsigned int s)
{
	seed = s;
}
