
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uchar;


enum { ERR_BMP_OK, ERR_BMP_NOBMP, ERR_BMP_COMPRESS, ERR_BMP_NOTFOUND };

int read_bmp(char *fname, void **dest, int dest_bpp, int *w, int *h, int alpha, int header_size);

int main(int argc, char **argv)
{
	FILE *fp;
	uchar *pix;
	int i,width, height;

	/* Read BMP and find borders */
	if(read_bmp(argv[1], &pix, 8, &width, &height, 0, 0) == ERR_BMP_OK)
	{
		for(i=0; i<width*height; i++)
			pix[i] += 16;

		if(fp = fopen(argv[2], "wb"))
		{
			fwrite(&width, 1, 1, fp);
			fwrite(&height, 1, 1, fp);
			fwrite(pix, 1, width*height, fp);
			fclose(fp);
		}
	}
}