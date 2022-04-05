
#include <pogo.h>
#include "text.h"
#include "misc.h"

/* Translatable text strings loaded from .shell/texts */
char *text_strings[TEXT_END];

int read_texts(FILE *fp)
{
	int rc;
	char *p;
	char tmp[256];
	/* Read the translatable texts */
	int l,i = 0;

	if(find_section(fp, "texts"))
	{
		while((rc = read_line(tmp, 256, fp)) >= 0)
		{
			if(rc > 0)
			{
				char *q = tmp;
				p = tmp;

				if(i == TEXT_END)
					fprintf(stderr, "TOO MANY TEXTS!\n");

				while(*p && (*p != 10) && (*p != 13))
				{
					if(*p == '\\')
					{
						if(p[1] == 'n')
						{
							*q++ = 10;

						}
						p += 2;
					}
					else
						*q++ = *p++;
				}
				*q = 0;
				l = strlen(tmp);
				text_strings[i] = malloc(l+1);
				strcpy(text_strings[i], tmp);
				i++;
			}
		}
	}
	else
	{
		/* Freeze if no texts are present */
		while(1);
	}

	return 1;
}

