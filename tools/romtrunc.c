#include <stdio.h>

#define BUF_SIZE 65536

int main(int argc, char **argv)
{
	int l;
	FILE *fpi, *fpo;
	unsigned char buf[BUF_SIZE];

	if(argc < 3)
	{
		printf("Truncate similar bytes at end of file\nUsage: romtrunc <infile> <outfile>\n");
		return 0;
	}
	
	fpi = fopen(argv[1], "rb");
	fpo = fopen(argv[2], "wb");

	if(fpi)
	{
		unsigned char c, oldc;
		fseek(fpi, -1, SEEK_END);
		fread(&c, 1, 1, fpi);
		oldc = c;
		while((!c || (c == 0xFF)) && (oldc == c))
		{
			fseek(fpi, -2, SEEK_CUR);
			oldc = c;
			fread(&c, 1, 1, fpi);
		}
		l = ftell(fpi);
		fseek(fpi, 0, SEEK_SET);
		printf("Truncating at %d\n", l);

		if(fpo)
		{
			while(l > BUF_SIZE)
			{
				fread(buf, 1, BUF_SIZE, fpi);
				fwrite(buf, 1, BUF_SIZE, fpo);
				l -= BUF_SIZE;
			}
			fread(buf, 1, l, fpi);
			fwrite(buf,1, l, fpo);
			fclose(fpi);
			fclose(fpo);
		}
	}
	return 0;
}