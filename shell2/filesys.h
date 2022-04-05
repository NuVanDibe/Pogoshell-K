#ifndef FILESYS_H
#define FILESYS_H

typedef struct
{
	char name[32];
	unsigned int size;
	unsigned int type;
}  __attribute__ ((packed)) DirList;


void filesys_clearkey(void);
void filesys_update_key(int bits);
int filesys_change_hidden_dir(void);
void filesys_dothide(int hide);
int filesys_getfiles(DirList *list);
int filesys_parent(void);
int filesys_back(void);
int filesys_enter(int i);
char *filesys_fullname(int i);
int filesys_cd(char *name);
char *filesys_get_current(void);

enum { FSTATE_NORMAL, FSTATE_ROOT, FSTATE_SRAM, FSTATE_GAMES };

int filesys_getstate(void);

#endif
