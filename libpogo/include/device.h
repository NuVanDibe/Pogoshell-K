#ifndef DEVICE_H
#define DEVICE_H

#include "core.h"

typedef void DIR;

struct dirent
{
	char d_name[32];
	uint32 d_size;
};

typedef struct
{
	char name[32];
	int size;
	int start;

} __attribute__ ((packed)) Romfile;

struct stat 
{
  uchar		st_dev;
  uint16	st_ino;
  uint16	st_mode;
  uchar		st_nlink;
  uchar		st_uid;
  uchar		st_gid;
  uchar		st_rdev;
  uint32	st_size;
  time_t	st_atime;
  time_t	st_mtime;
  time_t	st_ctime; 
};


#define S_IFDIR 16384

typedef struct {
	int (*open)(const char *name, int flags);
	int (*read)(int fd, void *dest, int size);
	int (*write)(int fd, const void *dest, int size);
	int (*close)(int fd);
	int (*lseek)(int fd, int offset, int origin);
	int (*ioctl)(int fd, int request, va_list vlist);
	int (*stat)(const char *path, struct stat *buffer);
	int (*remove)(const char *name);
	int (*rename)(const char *oldname, const char *newname);
	int (*readdir_r)(DIR *dir, struct dirent *entry, struct dirent **result);
} Device;

int device_register(Device *dev, char *name, void (*irqfunc)(void), int fd);

int open(const char *name, int flags);
int read(int fd, void *dest, int size);
int write(int fd, const void *dest, int size);
int close(int fd);
int lseek(int fd, int offset, int origin);
int tell(int fd);
int ioctl(int fd, int request, ...);
int remove(const char *name);
int readdir_r(DIR *dir, struct dirent *entry, struct dirent **result);
int stat(const char *name, struct stat *buffer);

void execv(const char *cmdname, const char *const *argv);
void execv_jump(const char *cmdname, const char *const *argv, void *jump);


#define	SEEK_SET	0	/* set file offset to offset */
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#define SEEK_MEM    3   /* Return memory position if a romfile */

#define O_CREAT 1

/* ioctl commands */

/* Standard ioctl commands */
#define IO_GETMODE 1
#define IO_SETMODE 2
#define IO_GETPARAM 3
#define IO_SETPARAM 4

/* Output console ioctl commands */
#define CC_GETWIDTH 5
#define CC_CLEAR 6
#define CC_SETFONT 9
#define CC_GETXY 10
#define CC_GETLINE 11
#define CC_GETFONT 13

/* Input console ioctl commands */
#define KC_SETIMAGE 9
#define KC_GETSCREENOFFSET 10
#define KC_ADDHISTORY 5
#define KC_SETDICT 6
#define KC_BINDKEY 7
#define KC_SETCOMPLETION 8

/* SRAM ioctl commands */
#define SR_SETUSER 20
#define SR_GETUSER 21
#define SR_SETUSERLIST 22
#define SR_GETUSERLIST 23
#define SR_FREESPACE 24

/* ROM ioctl commands */
#define RM_SETKEY 25
#define RM_VERIFYKEY 26
#define RM_GETKEY 27
#define RM_ALREADYHIDDEN 28

/* Parameters */

#define CM_LINEWRAP 1
#define CM_SHOWCURSOR 2
#define CM_CURSORFOLLOW 4

#define KM_RAW 1
#define KM_SHOWKBD 2
#define KM_BLOCKED 4
#define KM_ECHO 8

#define RAWKEY_L 1
#define RAWKEY_R 2
#define RAWKEY_UP 3
#define RAWKEY_DOWN 4
#define RAWKEY_LEFT 5
#define RAWKEY_RIGHT 6
#define RAWKEY_START 7
#define RAWKEY_SELECT 8
#define RAWKEY_A 9
#define RAWKEY_B 10

#define CHAR_LEFT 241
#define CHAR_RIGHT 242
#define CHAR_LINEKILL 250

#endif
