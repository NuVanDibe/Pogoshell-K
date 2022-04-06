
#include <pogo.h>
#include "settings.h"
#include "filesys.h"
#include "iwram.h"
#include "filetype.h"
#include "text.h"
#include "msgbox.h"

#define MARKED_STACK_DEPTH 48

extern tbox *DialogBox;
extern uint16 marked;

//static int dot_hide = 0;
static char current[128];
#if 0
static char return_dir[128];
#endif
static uint16 marked_stack[MARKED_STACK_DEPTH];
static char currtemp[128];
static DirList *lastlist;
static uint64 key[2];

int current_marked_level = 0;

/*
static char *tmp_item;
static int item_size;
static int (*cmp_func)(void *a, void *b);

static int partition(char *a, int low, int high)
{
	int left, right, pivot;
	char *pivot_item;

	pivot_item = &a[low * item_size];
	pivot = left = low;
	right = high;

	while(left < right)
	{
		// Move left while item < pivot
		while(cmp_func(&a[left * item_size], pivot_item) <= 0) left++;
		// Move right while item > pivot
		while(cmp_func(&a[right * item_size], pivot_item) > 0) right--;
		if(left < right)
		{
			// Swap elements
			memcpy(tmp_item, &a[left * item_size], item_size);
			memcpy(&a[left * item_size], &a[right * item_size], item_size);
			memcpy(&a[right * item_size], tmp_item, item_size);
		}
	}
	// right is final position for the pivot
	memcpy(tmp_item, pivot_item, item_size);
	memcpy(&a[low * item_size], &a[right * item_size], item_size);
	memcpy(&a[right * item_size], tmp_item, item_size);

	// low -> right-1 AND right+1 -> high

	if((right-1) > low)
		partition(a, low, right-1);

	if(high > (right+1))
		partition(a, right+1, high);

	return right;
}

void qsort(void *array, int count, int size, int cf(void *a, void *b))
{
	cmp_func = cf;
	item_size = size;
	tmp_item = malloc(size);
	partition((char *)array, 0, count-1);
	free(tmp_item);
}
*/

static int get_dir(char *name, DirList *list)
{
	int i;
	char *p;
	DIR *dir;
	struct dirent *result;
	struct stat sbuf;

	i = 0;
	dir = opendir(name);
	if(dir)
	{
		for(i=0; ((i < MAX_FILE_COUNT) &&
					!(readdir_r(dir, &list[i].entry, &result)) &&
					(result != NULL)); i++)
		{
			if(settings_get(SF_HIDEDOT) && list[i].entry.d_name[0] == '.')
				i--;
			else {
				p = &name[strlen(name)];
				*p = '/';
				strcpy(p+1, list[i].entry.d_name);
				stat(name, &sbuf);
				//fprintf(stderr, "get_dir(%s, %p); // (%s, %d)\n", name, list, list[i].entry.d_name, list[i].entry.d_size);
				//fprintf(stderr, "stat(%s, ...); // (..., %hd, ...)\n", name, sbuf.st_mode);
				*p = '\0';
				list[i].type = (sbuf.st_mode & S_IFDIR) ? 1 : 0;
			}
		}
		closedir(dir);

		return i;
	}

	return -1;
}
/*
void filesys_dothide(int hide)
{
	dot_hide = hide;
}
*/
int dl_cmp(DirList *a, DirList *b)
{
	if(a->type == b->type)
	{
		switch(settings_get(SF_SORTING))
		{
		case SORT_SIZE:
			return a->entry.d_size - b->entry.d_size;
		case SORT_TYPE:
			return (filetype_lookup(a) - filetype_lookup(b));
		default:
			return (strcmp(a->entry.d_name, b->entry.d_name));
		}
	}
	else
		return (b->type - a->type);
}

int fs_state = FSTATE_NORMAL;

int filesys_getstate(void)
{
	return fs_state;
}

// 7, 6, 5 repeating to fill up 126-bits (top two bits set)
void filesys_clearkey(void)
{
	key[0] = 0xFD7EBF5FAFD7EBF5ll;
	key[1] = 0xFEBF5FAFD7EBF5FAll;
}

// keep top two bits as zero
void filesys_update_key(int bits)
{
	if (bits < 0 || bits > 3)
		return ;
	key[1] = ((key[1] << 2) | (key[0] >> 62)) & 0x3FFFFFFFFFFFFFFFll;
	key[0] = (key[0] << 2) | bits;
}

int filesys_change_hidden_dir(void)
{
	char tmp[128];
	int rfd = open(current, 0);

	if (rfd >= 0) {
		//Hidden dirs can't be nested
		if (!ioctl(rfd, RM_ALREADYHIDDEN))
		{
			strcpy(tmp, current);
			strcat(tmp, "/<hidden>");
			if (ioctl(rfd, RM_VERIFYKEY, tmp, key))
			{
				ioctl(rfd, RM_SETKEY, key);
				close(rfd);
				return filesys_cd(tmp, marked);
			}
		}
		close(rfd);
	}
	return 0;
}

int filesys_getfiles(DirList *list)
{
	int c;
	lastlist = list;

	filesys_clearkey();
	if(*current == 0)
	{
		fs_state = FSTATE_ROOT;
		strcpy(list[0].entry.d_name, "rom");
		list[0].entry.d_size = 0;
		list[0].type = 2;
		strcpy(list[1].entry.d_name, "sram");
		list[1].entry.d_size = 0;
		list[1].type = 2;
		strcpy(list[2].entry.d_name, "cartroms");
		list[2].entry.d_size = 0;
		list[2].type = 2;
		return 3;
	}
	else
	{
		if(strncmp(current, "/sram", 5) == 0)
			fs_state = FSTATE_SRAM;
		else
		if(strncmp(current, "/cartroms", 6) == 0)
			fs_state = FSTATE_GAMES;
		else
			fs_state = FSTATE_NORMAL;

		if (DialogBox)
			msgbox_transient_show(DialogBox, TEXT(LOADING_LIST), TEXT(PLEASE_WAIT));
		c = get_dir(current, list);

		if(c > 0 && settings_get(SF_SORTING) != SORT_NONE)
			merge_sort(list, c, sizeof(DirList), (int (*)(void *, void *))dl_cmp);

		if (DialogBox)
			msgbox_transient_hide(DialogBox);
/*
		if(c > 0)
			qsort(list, c, sizeof(DirList), (int (*)(void *, void *))dl_cmp);
*/
	}

	return c;
}

char *filesys_get_current(void)
{
	return current;
}


int filesys_parent(void)
{
	char *p = strrchr(current, '/');
#if 0
	*return_dir = 0;
#endif

	//if(p)
	//	fprintf(stderr, "%s => %s\n", current, p);
	//else
	//	fprintf(stderr, "FAILED\n");

	if(p)
	{
		*p = 0;
		current_marked_level--;
		if (current_marked_level < MARKED_STACK_DEPTH)
			marked = marked_stack[current_marked_level];
		else
			marked = 0;
		return 0;
	}

	return -1;
}

#if 0
int filesys_back(void)
{
	if(*return_dir)
	{
		strcpy(current, return_dir);
		*return_dir = 0;
		return 1;
	}

	return filesys_parent();

}
#endif

#if 0
int filesys_enter(int i)
{
	if(i < 0) return NULL;
	if(lastlist[i].type == 1)
	{
		//fprintf(stderr, "ENTER from %s\n", current);
		strcat(current, "/");
		strcat(current, lastlist[i].name);
		//fprintf(stderr, "ENTER to %s\n", current);
		return 1;
	}
	return 0;
}
#endif

char *filesys_fullname(int i)
{
	if(i < 0) return NULL;
	strcpy(currtemp, current);
	strcat(currtemp, "/");
	strcat(currtemp, lastlist[i].entry.d_name);
	return currtemp;
}

int filesys_cd_marked_current(void)
{
	return filesys_cd_marked(current);
}

int filesys_cd_marked(char *name)
{
	char *p;
	int slash = 0;

	if (filesys_cd(name, 0)) {
		p = current;
		while (*p) {
			if (*p == '/') {
				if (slash < MARKED_STACK_DEPTH)
					marked_stack[slash] = 0;
				slash++;
			}
			p++;
		}
		current_marked_level = slash;
		return 1;
	}
	return 0;
}

int filesys_cd(char *name, uint16 prev_marked)
{
	DIR *dir;

	if((dir = opendir(name)))
	{
		closedir(dir);
#if 0
		strcpy(return_dir, current);
#endif
		strcpy(current, name);
		if (current_marked_level < MARKED_STACK_DEPTH)
			marked_stack[current_marked_level] = prev_marked;
		current_marked_level++;
		marked = 0;
		return 1;
	}
	//fprintf(stderr, "Couldnt open %s\n", name);
	return 0;
}
