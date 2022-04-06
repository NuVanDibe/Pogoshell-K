
#include <pogo.h>

#include "window.h"
#include "widgets/widgets.h"
#include "misc.h"

typedef void *(*WidgetAttrFunc)(Widget *, int, void *);

extern ListView *MainList;
extern TextBar *StatusBar;
extern TextBar *TitleBar;
extern BitMap *IconSet;
extern int IconHeight;

typedef struct _HashEntry
{
	int val;
	struct _HashEntry *next;
	char str[0];

} HashEntry;


static unsigned int hash_string(unsigned char *str)
{
	unsigned int hash = 0;
	while(*str)
		hash = ((hash<<5)^(hash>>27))^*str++;

	return hash;
}

HashEntry *symbols[41];

void symbol_init(void)
{
	memset(symbols, 0, 41*4);
}

void symbol_add(char *name, int val)
{
	HashEntry **he;
	int i = hash_string(name) % 41;

	he = &symbols[i];
	while(*he)
		he = &((*he)->next);

	*he = malloc(sizeof(HashEntry) + strlen(name) + 1);
	strcpy((*he)->str, name);
	(*he)->val = val;
	(*he)->next = NULL;
}

int symbol_get(char *name, int *val)
{
	HashEntry *he = symbols[hash_string(name) % 41];
	while(he && strcmp(he->str, name) != 0)
		he = he->next;

	if(he)
		*val = he->val;

	return (int)he;
}

int isspace(char c)
{
	return (c == ' ' || c == '\t');
}

Widget *guiparser_findwidget(char *name)
{
	int val = 0;
	symbol_get(name, &val);
	return (Widget *)val;

}

void guiparser_readsymbols(FILE *fp)
{
	char line[80];
	char *p, *sym, *val;
	int i,rc;

	if(find_section(fp, "symbols"))
	{
		symbol_init();

		while((rc = read_line(line, sizeof(line), fp)) >= 0)
		{
			if(rc > 0)
			{
				if(strchr(line, '='))
				{
					p = line;
					while(!isalpha(*p)) p++;
					sym = p;
					while(*p != '=') p++;
					*p++ = 0;
					val = p;
					while(isdigit(*p)) p++;
					*p = 0;

					i = atoi(val);
					symbol_add(sym, i);
				}
			}
		}
	}
}

int gethex(char *p)
{
	int l = 0;

	while(*p >= '0' && *p <= 'F')
	{
		if(*p <= '9')
			l = (l << 4) | ((*p++) - '0');
		else
			l = (l << 4) | ((*p++) - 'A' + 10);
	}
		
	return l;
}


void icons_set_attr(Widget *w, int attr, void *val)
{
	switch(attr & 0xFF0)
	{
		case WATR_BITMAP:
			if (IconSet)
				free(IconSet);

			IconSet = (BitMap *)val;
			break;
		case WATR_HEIGHT:
			IconHeight = (int)val;
			//fprintf(stderr, "IconHeight %d\n", IconHeight);
			break;
	}
}

Widget *guiparser_create(char *spec, char *rootitem)
{
	int val;
	BitMap *bm;
	Font *font;

	WidgetAttrFunc attr_func = (WidgetAttrFunc)NULL;
	Widget *lastw = NULL;
	Widget *rootw = NULL;

	int attr;
	char tmp[32];
	char *d, *startp, *endp;
	char *p = spec;


	while(p && *p)
	{
		lastw = NULL;

		while(*p && (isspace(*p) || *p == 10 || *p == 13)) p++;

		if(*p)
		{
			startp = p;
			while(*p != 10) p++;
			endp = p;

			if(strncmp(startp, "BACK", 4) == 0)
			{
				lastw = (Widget *)backdrop_new(STYLE_BEVEL);
				attr_func = (WidgetAttrFunc)backdrop_set_attribute;
			}
			else
			if(strncmp(startp, "LIST", 4) == 0)
			{
				lastw = (Widget *)listview_new(2, MAX_FILE_COUNT, NULL);
				attr_func = (WidgetAttrFunc)listview_set_attribute;
			}
			else
			if(strncmp(startp, "SCROLL", 6) == 0)
			{
				lastw = (Widget *)scrollbar_new();
				attr_func = (WidgetAttrFunc)scrollbar_set_attribute;
			}
			else
			if(strncmp(startp, "TRI", 3) == 0)
			{
				lastw = (Widget *)tricontainer_new();
				attr_func = (WidgetAttrFunc)tricontainer_set_attribute;
			}
			else
			if(strncmp(startp, "TEXTB", 5) == 0)
			{
				lastw = (Widget *)textbar_new(NULL, 256);
				attr_func = (WidgetAttrFunc)textbar_set_attribute;
				
			}
			else
			if(strncmp(startp, "TEXTF", 5) == 0)
			{
				lastw = (Widget *)textflow_new(NULL, 512);
				attr_func = (WidgetAttrFunc)textflow_set_attribute;
			}
			else
			if(strncmp(startp, "ICONS", 5) == 0)
			{

				attr_func = (WidgetAttrFunc)icons_set_attr;
				lastw = (Widget *)1;
				//fprintf(stderr, "Got IconSet\n");
			}

			p = startp;
			while(isalpha(*p)) p++;
			while(isspace(*p)) p++;

			//fprintf(stderr, "Found Widget\n");

			if(!lastw)
				p = NULL;
		}

		// Go on with attributes
		while(lastw)
		{
			startp = p;
			while(*p != '=') p++;
			strcpy(tmp, "WATR_");
			strncpy(&tmp[5], startp, p-startp);
			symbol_get(tmp, &attr);

			p++;

			//fprintf(stderr, "Attribute %s (%c) %d\n", tmp, *p, attr);

			startp = p;

			switch(*p)
			{
			case '<':
				p++;
				d = tmp;
				while(*p != '>')
					*d++ = *p++;
				*d = 0;

				d--;
				while(d > tmp && *d && *d != '.') d--;

				if(*d == '.')
					d++;
				else
					d = tmp;

				p++;

				//fprintf(stderr, "Opening file %s %s\n", tmp, d);

				//if(())
				{
					if(*d == 'b')
					{
						//fp = fopen(tmp, "rb");
						//bm = bitmap_readbm(fp);
						//fclose(fp);
						bm = bitmap_loadbm(tmp);
						attr_func(lastw, attr, bm);
					}
					else
					if(*d == 'f')
					{
						font = font_load_path(tmp);
						font->flags |= FFLG_TRANSP;
						attr_func(lastw, attr, font);
					}
					//fclose(fp);
				}
					
				break;
			case '\"':
				p++;
				d = tmp;
				while(*p != '\"')
					*d++ = *p++;
				*d = 0;
				p++;
				attr_func(lastw, attr, tmp);
				break;
			case '$':
				val = gethex(++p);
				while(isalnum(*p)) p++;
				attr_func(lastw, attr, (void *)val);
				break;
			default:
				if(isalpha(*p))
				{
					//p++;
					d = tmp;
					while(isalnum(*p))
						*d++ = *p++;
					*d = 0;
					p++;
					if(attr == WATR_NAME)
					{
						//fprintf(stderr, "Widget \"%s\" %p\n", tmp, lastw);
						symbol_add(strdup(tmp), (int)lastw);
						/*if(strcmp(tmp, "list") == 0)
							MainList = (ListView *)lastw;
						else
						if(strcmp(tmp, "status") == 0)
							StatusBar = (TextBar *)lastw;
						else
						if(strcmp(tmp, "title") == 0)
							TitleBar = (TextBar *)lastw;
						else*/
						if(strcmp(tmp, rootitem) == 0)
							rootw = lastw;
					}
					else
					{
						symbol_get(tmp, &val);
						attr_func(lastw, attr, (void *)val);
					}
				}
				else
				{
					val = atoi(p);
					while(isalnum(*p)) p++;
					attr_func(lastw, attr, (void *)val);
				}
				break;
			}

			//p = startp;

			while(*p && *p != 10 && !isalpha(*p)) 
			{
				p++;
			}

			if(!(*p) || *p == 10)
				lastw = NULL;
		}
	}

	return rootw;

}

