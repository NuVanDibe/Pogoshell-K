#include <stdio.h>
#include <stdlib.h>

int colsize = -1; //80;
int indent = 2;
int lfcount = 0;

/*

  TXT2HTML

  FIRST Unwraps wrapped lines.

  Remove all linefeeds except where selected occurences of following occurs

  - First word on next line would have fitted on current line
  - Spaces in beginning of next line
  - More than one linefeed in a row

  THEN HTML:izes

*/



int main(int argc, char **argv)
{
	char *src, *dst, *inbuf, *outbuf, *linep, *lined;
	int l,i;
	char *srcfile = NULL;
	char *dstfile = NULL;

	int spaces = 0;
	int lf = 0;
	FILE *fp;


	for(i=1; i<argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'l':
				lfcount = atoi(&argv[i][2]);
				break;
			case 'i':
				indent = atoi(&argv[i][2]);;
				break;
			case 'c':
				colsize = atoi(&argv[i][2]);
				printf("Column size set to %d\n", colsize);
				break;
			}
		}
		else
		{
			if(!srcfile)
				srcfile = argv[i];
			else
			if(!dstfile)
				dstfile = argv[i];
		}
	}


	fp = fopen(srcfile, "rb");
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
			if(lf)
			{ 
				if((indent > 0) && (spaces >= indent))
				{
					*dst++ = 9;
					printf("%d ", spaces);
				}
				else
				if(lfcount && lf >= lfcount)
					*dst++ = 13;

				*dst++ = 10;
				lf = spaces = 0; 
			}
			if(c >= 0x20)
			{
				if(spaces) { *dst++ = ' '; spaces = 0; }
				*dst++ = c;
			}
		}
	}
	*dst = 0;

	src = outbuf;
	dst = inbuf;

	printf("Unwrapping\n");

	// PASS 2 - Unwrap
	while(*src)
	{
		char *s;
		char c = *src++;

		//printf("%c ", c);
		switch(c)
		{

		case 10:
			s = src;
			while(*s && *s != ' ') s++;

			if( (colsize > 0) && ((src-linep) + (s-src) < colsize)) 
				*dst++ = 10;
			else
				*dst++ = ' ';

			linep = src;
			lined = dst;
			break;
	
		case 9:
			*dst++ = 10;
			src++;
			break;

		case 13:
			*dst++ = 10;
			src++;
			break;

		default:
			*dst++ = c;
			break;
		}
	}
	*dst = 0;

	src = inbuf;
	dst = outbuf;

	strcpy(dst, "<html>\n<p>");
	dst += strlen(dst);

	printf("HTML:izing\n");

	while(*src)
	{
		char c = *src++;

		if(c == 10)
		{
			strcpy(dst, "</p>\n<p>");
			dst += strlen(dst);
		}
		else
			*dst++ = c;

	}
	
	strcpy(dst, "</p>\n</html>\n");
	dst += strlen(dst);

	*dst = 0;

	fp = fopen(dstfile, "wb");
	fwrite(outbuf, dst-outbuf, 1, fp);
	fclose(fp);

	return 0;
}