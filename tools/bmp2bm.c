
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
	uint16 *pix;
	uint16 w, h, bpp;
	char head[2] = "BM";
	int i,width, height;

	if(read_bmp(argv[1], &pix, 16, &width, &height, 0, 0) == ERR_BMP_OK)
	{
		w = width;
		h = height;
		bpp = 16;

		for(i=0; i<width*height; i++)
		{
			if(pix[i] == 0x03E0)
				pix[i] |= 0x8000;
		}

		if(fp = fopen(argv[2], "wb"))
		{
			fwrite(&head, 2, 1, fp);
			fwrite(&bpp, 2, 1, fp);
			fwrite(&w, 2, 1, fp);
			fwrite(&h, 2, 1, fp);
			fwrite(pix, 2, width*height, fp);
			fclose(fp);
		}
	}
}