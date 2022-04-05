
#include <pogo.h>
#include "settings.h"
#include "filesys.h"
#include "filetype.h"

//static int dot_hide = 0;
static char current[128];
static char return_dir[128];
static char currtemp[128];
static DirList *lastlist;
static uint64 key[2];

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

void merge_sort(void *array, int count, int size, int cf(void *a, void *b))
{
	int i, join, actualsize;
	int left, middle;
	char *a = (char *)array;
	char *tmp_item = malloc(size);

	for (i = 0; i < count; i += 2)
	{
		if (i < count - 1)
		{
			if (cf(&a[i * size], &a[(i + 1) * size]) > 0) {
				memcpy(tmp_item, &a[i * size], size);
				memcpy(&a[i * size], &a[(i+1) * size], size);
				memcpy(&a[(i+1) * size], tmp_item, size);
			}
		}
	}
	for (i = 4; i < count*2; i *= 2)
	{
		for (join = 0; join < count; join += i)
		{
			actualsize = count - join;
			if (actualsize > i)
				actualsize = i;
			left = join;
			middle = join + i/2;
			while (left < middle && middle < join+actualsize) {
				if (cf(&a[left * size], &a[middle * size]) > 0) {
					memcpy(tmp_item, &a[middle * size], size);
					memmove(&a[(left+1) * size], &a[left * size], size*(middle-left));
					memcpy(&a[left * size], tmp_item, size);
					middle++;
				}
				left++;
			}
		}
	}
	free(tmp_item);
}

static int get_dir(char *name, DirList *list)
{
	int i;
	char *p;
	DIR *dir;
	struct dirent *de;
	struct stat sbuf;
	DirList *dl;

	i = 0;
	dir = opendir(name);
	if(dir)
	{
		for(i=0; ((de = readdir(dir)) && i<MAX_FILE_COUNT); i++)
		{
			dl = &list[i];
			strcpy(dl->name, de->d_name);
			p = &name[strlen(name)];
			*p = '/';
			strcpy(p+1, de->d_name);
			stat(name, &sbuf);
			//dl->size = sbuf.st_size;
			dl->size = de->d_size;
			*p = 0;
			if(settings_get(SF_HIDEDOT) && de->d_name[0] == '.')
				i--;
			else
			{
				if(sbuf.st_mode & S_IFDIR)
					dl->type = 1;
				else
					dl->type = 0;
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
			return a->size > b->size;
		case SORT_TYPE:
			return (filetype_lookup(a) > filetype_lookup(b));
		default:
			return (strcmp(a->name, b->name));
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
				return filesys_cd(tmp);
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
		strcpy(list[0].name, "rom");
		list[0].size = 0;
		list[0].type = 2;
		strcpy(list[1].name, "sram");
		list[1].size = 0;
		list[1].type = 2;
		strcpy(list[2].name, "cartroms");
		list[2].size = 0;
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

		c = get_dir(current, list);

		if(c > 0)
			merge_sort(list, c, sizeof(DirList), (int (*)(void *, void *))dl_cmp);
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
	*return_dir = 0;

	//if(p)
	//	fprintf(stderr, "%s => %s\n", current, p);
	//else
	//	fprintf(stderr, "FAILED\n");

	if(p)
	{
		*p = 0;
		return 1;
	}

	return 0;
}

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

char *filesys_fullname(int i)
{
	if(i < 0) return NULL;
	strcpy(currtemp, current);
	strcat(currtemp, "/");
	strcat(currtemp, lastlist[i].name);
	return currtemp;
}

int filesys_cd(char *name)
{
	DIR *dir;

	if((dir = opendir(name)))
	{
		closedir(dir);
		strcpy(return_dir, current);
		strcpy(current, name);
		return 1;
	}
	//fprintf(stderr, "Couldnt open %s\n", name);
	return 0;
}
