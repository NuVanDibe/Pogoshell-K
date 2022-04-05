#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *src, *dst, *inbuf, *outbuf, *linep;
	int l;
	int spaces = 0;
	int lf = 0;

	FILE *fp = fopen(argv[1], "rb");
	fseek(fp, 0, SEEK_END);
	l = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	src = inbuf = malloc(l*4+1);
	dst = outbuf = malloc(l*4);
	fread(inbuf, l, 1, fp);
	fclose(fp);
	inbuf[l] = 0;

	src = inbuf;

	// PASS 1 - Whitespace strip
	while(*src)
	{
		char c = *src++;
		switch(c)
		{
		case ' ':
			spaces++;
			break;
		case 10:
			lf++;
			spaces = 0;
			break;
		case 13:
			break;
		default:
			if(lf) { *dst++ = 10; lf = spaces = 0; }
			if(spaces) { *dst++ = ' '; spaces = 0; }
			*dst++ = c;
		}
	}
	*dst = 0;

	src = outbuf;
	dst = inbuf;

	// PASS 2 - Unwrap
	while(*src)
	{
		char c = *src++;
		if(c == 10)
		{
			char *s = src;
			while(*s != ' ') s++;

			if( (src-linep) + (s-src) < 80)
			{
				strcpy(dst, "\x0a<P>\x0a");
				dst += 5;
				//*dst++ = 10;
			}
			else
				*dst++ = ' ';

			linep = src;
		}
		else
			*dst++ = c;
	}

	fp = fopen(argv[2], "wb");
	fwrite(inbuf, dst-inbuf, 1, fp);
	fclose(fp);

	return 0;
}