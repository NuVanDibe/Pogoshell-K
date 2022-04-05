#include <stdio.h>

#define BUF_SIZE 65536



int main(int argc, char **argv)
{
	int l, count;
	FILE *fpi, *fpo;
	unsigned char patch[256];
	unsigned char buf[BUF_SIZE];

	fpi = fopen(argv[3], "rb");
	count = fread(patch, 1, 256, fpi);
	fclose(fpi);
	
	fpi = fopen(argv[1], "rb");
	fpo = fopen(argv[2], "wb");

	if(fpi && fpo)
	{
		int rc = 1;
		int first = 1;
		while(rc > 0)
		{
			rc = fread(buf, 1, BUF_SIZE, fpi);

			if(place > offset && place < (offset+rc))
			{
				opcode = *((uint *)&buf[place-offset]);
				jumper = (opcode & 0x7FF) | (((opcode>>16) & 0x07FF)<<11);
				printf("Branch to %x (offset %x)\n", place + jumper*2, jumper);
			}

			offset += rc;

			//if(first)
			//	memcpy(buf, patch, count);

			if(rc)
				fwrite(buf, 1, rc, fpo);
			first = 0;
		}
		fclose(fpi);
		fclose(fpo);
	}
	
	return 0;
}