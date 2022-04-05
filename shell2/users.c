
#include <pogo.h>
#include "users.h"
#include "misc.h"
#include "msgbox.h"
int CurrentUser;
char *UserName;

typedef struct
{
	char *name;
	unsigned char uid;

} User;

static User *users[10];
static int ucount = 0;


void update_user(void)
{
	int i;
	for(i=0; i<ucount; i++)
	{
		if(users[i]->uid == CurrentUser)
		{
			UserName = users[i]->name;
			return;
		}
	}
}

int read_users(FILE *fp)
{
	char line[64];
	char *ptr;
	int ti, rc;
	char *tokens[4];
	User *u;

	if(find_section(fp, "users"))
	{
		while((rc = read_line(line, sizeof(line), fp)) >= 0)
		{
			if(rc > 0)
			{
				ptr = line;
				ti = 0;
				while(*ptr)
				{
					tokens[ti++] = ptr;
					while(*ptr && *ptr != ':') ptr++;
					if(*ptr)
						*ptr++ = 0;
				}

				u = malloc(sizeof(User));
				users[ucount++] = u;
				u->name = strdup(tokens[0]);
				u->uid = atoi(tokens[1]);
				//fprintf(stderr, "User '%s' %d\n", u->name, u->uid);
			}
		}
	}

	CurrentUser = users[0]->uid;
	UserName = users[0]->name;

	sram_setuser(CurrentUser);

	return 0;
}


int switch_user(void)
{
	int rc,i;
	char *names[10];

	for(i=0; i<ucount; i++)
		names[i] = users[i]->name;

	rc = msgbox_list("Switch User", names, ucount);

	if(rc >= 0)
	{
		CurrentUser = users[rc]->uid;
		UserName = users[rc]->name;
		sram_setuser(CurrentUser);
	}


	return 0;

}
