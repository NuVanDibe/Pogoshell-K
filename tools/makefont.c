/*
 * MAKEFONT sourcecode (makefont.c) - created by Jonas Minnberg 2002
 *
 * Creates fonts for pogo from BMP-files
 *
 * Part of the pogo distribution.
 * Do what you want with this but please credit me.
 * -- jonas@nightmode.org
 ****/

#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uchar;

#include "../libpogo/include/font.h"

enum { ERR_BMP_OK, ERR_BMP_NOBMP, ERR_BMP_COMPRESS, ERR_BMP_NOTFOUND };

int read_bmp(char *fname, void **dest, int dest_bpp, int *w, int *h, int alpha, int header_size);

int colorfont = 0;
int fixed = 0;
int spacing = 0;
int space = -1;
int charwidth = 0;
int first = -1;
int last = 127;

int main(int argc, char **argv)
{
	int i,width, height, x, y, b, lastb, sh;
	int lowy = 99999;
	int lowx = 99999;
	int hiy = 0;
	int hix = 0;
	unsigned short *pix;
	unsigned char *out, *outbuf;
	char *offslog = NULL;
	FILE *fp;
	int index;
	unsigned short c;
	unsigned short bgcol = 0;
	unsigned short fgcol = 0xFFFF;
	unsigned short *offsets, *offset_tab;
	unsigned short pal[256];
	int pcount = 0;

	Font font;

	int rem_count = 0;
	int add_count = 0;
	unsigned short add_offsets[128];
	unsigned short rem_offsets[128];

	if(argc < 3)
	{
		printf("Usage: makefont <bmpfile> <fontfile> [options] [offsets..]\n\n");
		printf("-f       Convert fixed font\n");
		printf("-c       Convert color font\n");
		printf("-s<num>  Set ASCII of first character\n");
		printf("-e<num>  Set ASCII of last characters\n");
		printf("-o<file> Dump offsets to logfile\n");
		printf("-w<num>  Character width (space for non-fixed)\n");
		printf("\n Any positive or negative numbers after options indicates\n");
		printf("offsets to add or remove to the offset list (for instance, chars\n");
		printf("containing space - like the double quote - will generate an extra\n");
		printf("offset that needs to be removed while chars packed to tight\n");
		printf("(without a space in between) will need an offset added).\n");
	}

	for(x=3; x< argc; x++) {
		if(*argv[x] == '-')
		{
			if(isdigit(argv[x][1]))
				rem_offsets[rem_count++] = atoi(&argv[x][1]);
			else
				switch(argv[x][1])
				{
				case 'f':
					fixed = 1;
					break;
				case 's':
					first = atoi(&argv[x][2]);
					break;
				case 'p':
					spacing = atoi(&argv[x][2]);
					break;
				case 'e':
					last = atoi(&argv[x][2]);
					break;
				case 'o':
					offslog = &argv[x][2];
					break;
				case 'w':
					charwidth = atoi(&argv[x][2]);
					space = 0;
					break;
				case 'c':
					colorfont = 1;
					break;
				}
		} else
		if(isdigit(*argv[x]))
			add_offsets[add_count++] = atoi(argv[x]);
	}

	/* Read BMP and find borders */
	if(read_bmp(argv[1], &pix, 16, &width, &height, 0, 0) == ERR_BMP_OK)
	{
		bgcol = *pix;
		printf("bgcol is %08x\n", bgcol);

		if(fixed) {
			lowx = lowy = 0;
			hix = width-1;
			hiy = height-1;
		}

		offsets = offset_tab = malloc(2 * width);

		if(!fixed)
		{
			/* Crop to miminum size */
			for(x=0; x<width; x++)
			{
				for(y=0; y<height; y++)
				{
					if(pix[x + y*width] != bgcol)
					{
						if(fgcol == 0xFFFF)
							fgcol = pix[x + y*width];

						if(x < lowx)
							lowx = x;
						if(y < lowy)
						{
							printf("lowy = %d\n", y);
							lowy = y;
						}
						if(x > hix)
							hix = x;
						if(y > hiy)
							hiy = y;
					}

				}
			}

			printf("Cutting to %d,%d -> %d,%d\n", lowx, lowy, hix, hiy);

			/* Find offsets to offsets table */
			for(lastb=0, x=lowx, sh=0; x<=hix; x++, sh++)
			{
				for(b=0, y=lowy; y<=hiy; y++)
				{
					b += (pix[x + y*width] == bgcol ? 0 : 1);
				}
				if(b && !lastb)
				{
					int o,i;
					o = x-lowx;

					/* Check if we should add an offset here */
					for(i=0; i<add_count; i++)
					{
						if(o > add_offsets[i] && offsets[-1] < add_offsets[i])
							*offsets++ = add_offsets[i];
					}

					/* Check if we should remove an offset here */
					for(i=0; i<rem_count; i++)
					{
						if(rem_offsets[i] == o)
							i = 9999;
					}
					if(i != 10000)
						*offsets++ = x-lowx;
				}
				lastb = b;
			}
		}

		out = outbuf = malloc(((width+31)) * height);

		// Copy cropped area

		if(colorfont)
		{
			pal[pcount++] = bgcol;

			for(y=lowy; y<=hiy; y++)
			{
				for(x=lowx, sh=0; x<=hix; x++, sh++)
				{
					c =  pix[x+y*width];
					index = -1;
					for(i=0; i<pcount; i++)
					{
						if(c == pal[i])
						{
							index = i;
							break;
						}
					}
					if(index == -1)
					{
						pal[pcount] = c;
						index = pcount++;
					}

					*out++ = index;
				}
			}
		}
		else
		{
			for(y=lowy; y<=hiy; y++)
			{
				for(x=lowx, sh=0; x<=hix; x++, sh++)
					*out++ = ((pix[x + y*width] == bgcol) ? 0 : 1);
			}
		}


		if(first == -1)
			first = (fixed ? ' ' : '!');

		/* Make sure we have offsets for all characters */
		b = offsets-offset_tab;
		if(!fixed && (b <= (last-first)))
			last = b+first-1;

		/* Calculate space size if not set */
		if(space == -1) {
			space = (hix-lowx+1)/(last-first);
		} else {
			if (charwidth < 0)
				space = (hix-lowx+1)/(last-first) + charwidth;
			else
				space = charwidth;
		}

		font.charwidth = space;
		font.first = first;
		font.last = last;
		font.flags = fixed ? 0 : 1;
		if(colorfont) font.flags |= FFLG_COLOR;
		font.spacing = spacing;
		font.width = hix-lowx+1;
		font.height = hiy-lowy+1;

		if((int)out & 1) out++;

		fp = fopen(argv[2], "wb");

		if(fp)
		{
			fwrite(&font, 8, 1, fp);
			if(colorfont)
			{
				fwrite(&pcount, 2, 1, fp);
				fwrite(pal, pcount*2, 1, fp);
			}
			fwrite(outbuf, 1, (out-outbuf), fp);
			if(!fixed)
				fwrite(offset_tab, 1, (offsets - offset_tab)*2, fp);
			fclose(fp);
		}

		if(offslog)
		{
			fp = fopen(offslog, "w");
			y = offsets - offset_tab;
			fprintf(fp, "OFFSET LOG\n");
			printf("Dumping %d offsets\n", y);
			for(x=0; x<y; x++) {
				fprintf(fp, "'%c' : %d\n", first+x, offset_tab[x]);
			}
			fclose(fp);
		}
		

		free(outbuf);
	}

	//scanf("%*s");
	return 0;
}
