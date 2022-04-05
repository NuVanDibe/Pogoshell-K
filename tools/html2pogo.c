#include <stdio.h>
#include <string.h>

/*
*/

int ignore = 0;

int spaces = 0;

void dospace(char **p)
{
	if(spaces)
		*(*p)++ = ' ';
	spaces = 0;
}


int check_align(char *p)
{
	if(p = strstr(p, "ALIGN"))
	{
		while(*p && *p != '=') p++;
		if(*p == '=')
		{
			p++;
			while(*p && *p == ' ') p++;
			if(*p == '"') p++;
			if(strncmp(p, "CENTER", 6) == 0)
			{
				return 11;
			}
			else
			if(strcmp(p, "RIGHT") == 0)
				return 14;
		}
	}
	return 13;
}

int big = 0;
int heading = 0;

enum {P_NONE, P_LINE, P_INDENT};

int paragraph = P_LINE;

int inpara = 0;

void handle_tag(char *tag, char **out)
{
	char *p;
	char *dest = *out;
	//printf("Tag <%s>\n", tag);
	//spaces = 0;
	if((strcmp(tag, "P") == 0) || 
	   (strncmp(tag, "P ", 2) == 0) ||
	   (strcmp(tag, "BLOCKQUOTE") == 0)
	   )
	{
		if(inpara)
		{
			*dest++ = 10;
			if(big) *dest++ = 10;
		}

		inpara = 1;
		*dest++ = check_align(tag);
		spaces = 0;
		//dospace(&dest);
		if(paragraph == P_LINE)
			*dest++ = 10;
		else
		if(paragraph == P_INDENT)
		{
			*dest++ = ' ';
			*dest++ = ' ';
			*dest++ = ' ';
			*dest++ = ' ';
		}
	}
	else
	if(strncmp(tag, "/P", 2) == 0)
	{
		*dest++ = 10;
		if(big) *dest++ = 10;
		inpara = 0;
	}
	else
	if(tag[0] == 'H' && isdigit(tag[1]))
	{

		heading = 1;

		spaces = 0;

		*dest++ = 10;
		*dest++ = 10;
		*dest++ = check_align(tag);

		if(tag[1] <= '2')
		{
			*dest++ = 4;
			big = 1;
		}
		else
			*dest++ = 2;
	}
	else
	if((strncmp(tag, "/H", 2) == 0) && (isdigit(tag[2])))
	{
		spaces = 0;
		//dospace(&dest);
		//NOBOLD(dest);

		*dest++ = 1;
		*dest++ = 10;
		if(big) *dest++ = 10;
		big = 0;

	}
	else
	if(stricmp(tag, "LI") == 0)
	{
		spaces = 0;
		//dospace(&dest);
		strcpy(dest, "\n  * ");
		dest += 5;
	}
	else
	if(strcmp(tag, "B") == 0)
	{
		dospace(&dest);
		*dest++ = 2;
		//BOLD(dest);
	}
	else
	if(strcmp(tag, "/B") == 0)
	{
		dospace(&dest);
		*dest++ = 1;
		//NOBOLD(dest);
	}
	if(strcmp(tag, "I") == 0)
	{
		dospace(&dest);
		*dest++ = 3;
		//BOLD(dest);
	}
	else
	if(strcmp(tag, "/I") == 0)
	{
		dospace(&dest);
		*dest++ = 1;
		//NOBOLD(dest);
	}
	else
	if(strcmp(tag, "BR") == 0)
	{
		*dest++ = 10;
		if(big) *dest++ = 10;
		spaces = 0;
		//BOLD(dest);
	}
	else
	if(stricmp(tag, "TITLE") == 0)
	{
		ignore = 1;
	}
	else
	if(stricmp(tag, "/TITLE") == 0)
	{
		ignore = 0;
	}

	*out = dest;
}

void handle_spec(char *s, char **out)
{
	char *dest = *out;
	if(strcmp(s, "quot") == 0)
		*dest++ = '\"';
	else
	if(strcmp(s, "nbsp") == 0)
		*dest++ = ' ';
	*out = dest;
}

int tag_mode = 0;
char *dest;

void parse_line(char *in, char *out)
{
	char *orgdest;
	char tag[1280];
	
	if(tag_mode)
		goto tag;

	while(*in)
	{
		orgdest = out;

		switch(*in)
		{
		case '&':
			if(spaces)
				*out++ = ' ';
			spaces = 0;

			in++;
			dest = tag;
			while(*in && *in != ';')
				*dest++ = *in++;
			*dest = 0;
			handle_spec(tag, &out);
			break;
		case '<':
			in++;
			dest = tag;
tag:
			while(*in && *in != '>')
			{
				*dest++ = toupper(*in++);
			}
			if(*in)
			{
				*dest = 0;
				handle_tag(tag, &out);
				tag_mode = 0;
			}
			else
				tag_mode = 1;
			break;
		case 0x97:
			if(spaces)
				*out++ = ' ';
			*out++ = '-';
			spaces = 0;
			break;
		case 0x94:
		case 0x95:
			if(spaces)
				*out++ = ' ';
			*out++ = '\"';
			spaces = 0;
			break;

		case ' ':
		case 10:
		case 13:
			spaces++;
			break;
		default:
			if(spaces)
				*out++ = ' ';
			*out++ = *in;
			spaces = 0;
			break;
		}
		in++;

		if(ignore)
			out = orgdest;

	}
	spaces++;
	*out = 0;
}


char buf1[16384];
char buf2[16384];

int main(int argc, char **argv)
{
	char tmp[64];
	char *srcfile = NULL;
	char *dstfile = NULL;
	char *pattern = NULL;
	int split = 0;
	int min_size = 0;
	int i;

	int count = 0;
	int chap = 1;

	FILE *fp1, *fp2;

	for(i=1; i<argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'p':
				pattern = &argv[i][2]; // Split on pattern
				break;
			case 's': // Match chapter (and split) on H?
				split = 1;
				break;
			case 'S':
				split = 2; // Split on H1 & H2
				break;
			case 'm':
				min_size = atoi(&argv[i][2]);
				printf("Minimum chapter size set to %d\n", min_size);
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

	
	fp1 = fopen(srcfile, "r");

	if(split)
		sprintf(tmp, "%s%d.txt", dstfile, chap);
	else
		strcpy(tmp, dstfile);
	fp2 = fopen(tmp, "wb");

	if(!fp1)
	{
		printf("No input file\n");
		return 0;
	}
	if(!fp2)
	{
		printf("No output file\n");
		return 0;
	}

	while(fgets(buf1, sizeof(buf1), fp1))
	{
		i = 1;
		buf1[strlen(buf1)-1] = 0;
		parse_line(buf1, buf2);

		if(pattern)
		{
			if(strstr(buf1, pattern))
				i = 2;
			else
				i = 0;
		}

		if(i && split)
		{
			if(heading)
				i = 2;
			else
				i = 0;
		}


		if((i == 2) && (count > min_size))
		{
			fclose(fp2);
			chap++;
			sprintf(tmp, "%s%d.txt", dstfile, chap);
			fp2 = fopen(tmp, "wb");
			count = 0;
		}

		heading = 0;
		fwrite(buf2, 1, strlen(buf2), fp2);
		count += (strlen(buf2));
	}
	fclose(fp1);
	fclose(fp2);
	return 0;
}
