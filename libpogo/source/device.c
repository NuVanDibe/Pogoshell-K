/*
 * Core device handling functions (font.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Functions for handling devices - allowing users to
 * register new devices that gets called through the
 * open(),close(),read() etc functions defined here.
 *
 **/

#include "core.h"
#include "device.h"
#include "gba_defs.h"

#include "cartlib.h"


typedef struct {
	uint16 fd;
	Device *dev;
} FdMap;

typedef struct {
	Device *device;
	char *name;
	volatile void (*irqfunc)(void);
} RegDevice;

#define MAX_DEVICE 8
#define MAX_FD 32

static int dev_count = 0;
static RegDevice deviceList[MAX_DEVICE];

static int fdmap_count = 3;
static FdMap fdList[MAX_FD];

void device_init(void)
{
	fdList[0].fd = -1;
	fdList[0].dev = NULL;
	fdList[1].fd = -1;
	fdList[1].dev = NULL;
	fdList[2].fd = -1;
	fdList[2].dev = NULL;
}

static Device *dev_fromname(const char *name, char **cutname)
{
	int i,l;
	char *cn = *cutname;
	for(i=0; i<dev_count; i++) {
		l = strlen(deviceList[i].name);
		if(strncmp(name, deviceList[i].name, l) == 0)
		{
			cn = (char *)&name[l];
			while(*cn == '/')
				cn++;
			*cutname = cn;
			return deviceList[i].device;
		}
	}
	*cutname = (char *)name;
	return deviceList[0].device;
}

static Device *dev_fromhandle(int *fd)
{
	Device *dev = fdList[*fd].dev;
	*fd = fdList[*fd].fd;
	return dev;
}

int device_register(Device *dev, char *name, volatile void (*irqfunc)(void), int fd)
{
	RegDevice *d = &deviceList[dev_count++];
	d->name = name;
	d->irqfunc = irqfunc;
	d->device = dev;

	if(fd > -1 && fd < 3)
	{
		fdList[fd].dev = dev;
		fdList[fd].fd = 0;
	}

	return dev_count-1;
}



/*
const unsigned short setters[] =
{
	0x000, 0x0080,
	0x004, 0x0000,
	//0x006, 0x0048,
	0x006, 0x0002, //
	0x020, 0x0100,
	0x026, 0x0100,
	0x030, 0x0100,
	0x036, 0x0100,
//	0x082, 0x0000	,
//	0x084, 0x0000,
	0x088, 0x0000,
	0x100, 0xFF86,
	0x134, 0x0009, //
	0x140, 0x0000,
	0x206, 0x0000,
};

const unsigned short clearers[] = 
{
	0x008,0x056,
	//0x040,0x04C,
	0x060,0x086,
	//0x088,0x08A,

	//0x080,0x086,
	0x090,0x0A8,
	0x0B0,0x0E0, // DMA

	0x100,0x110,
	0x120,0x12E,
	0x130,0x136,
	0x150,0x15A,
	0x200,0x206,
};

void reset_io(void)
{
	int i,j;
	volatile unsigned short *io = (void *)0x04000000;

	for(i=0; i<sizeof(clearers)/4; i++)
		for(j=clearers[i*2]; j<clearers[i*2+1]; j+=2)
			io[j/2] = 0;

	for(i=0; i<sizeof(setters)/4; i++)
		io[setters[i*2]/2] = setters[i*2+1];
}
*/

/* Rom execute routines - not sure if the should be in device.c */

extern void executeCart(uint32 a, uint32 b, uint32 c) IN_IWRAM;

static int jump_adress = 0;

void make_arguments(const char *cmdname, const char *const *argv)
{
	int fd;
	uint32 *p;
	uchar *ptr;
	int i = 0;
//	int l = 0;
	
/*	fd = open(cmdname, 0);
	if(fd >= 0)
	{
		l = lseek(fd, 0, SEEK_MEM);
		fprintf(stderr, "Found %s at %p\n", cmdname, l);
		close(fd);
	}
*/
	p = (uint32 *)(0x02000000+255*1024);
	p[0] = 0xFAB0BABE;	
	ptr = (uchar *)&p[2];
	strcpy(ptr, (uchar *) cmdname);
	ptr += (strlen(ptr)+1);
	while(argv && argv[i])
	{
		const char *s;
		if(strncmp(argv[i], "/rom", 4) == 0)
			s = &argv[i][4];
		else
			s = argv[i];

		strcpy(ptr, s);
		ptr += (strlen(ptr)+1);
		i++;
	}
	p[1] = i+1;
	
	//p = (uint32 *)(0x0203FC00-4);
	if(argv && argv[0] && (fd = open(argv[0], 0)))
	{
		int m = lseek(fd, 0, SEEK_MEM);
		int s = lseek(fd, 0, SEEK_END);
		//m -= (l - 0x08000000);
		//fprintf(2, "Found %s at %x\n", argv[0] , m);
		p[-1] = m;
		p[-2] = s;
		close(fd);
	} else
		p[-1] = 0;

}


void execv(const char *cmdname, const char *const *argv)
{
	uint32 *p;
	uint32 l;
	int i = 0;

	int fd = open(cmdname, 0);
	if(fd >= 0)
	{
		l = lseek(fd, 0, SEEK_MEM);
		close(fd);
		if((l&0x00007FFF) == 0)
		{

			make_arguments(cmdname, argv);

			// Remap for start position of rom
			p = (uint32 *)(0x02000000+255*1024);
			//if(p[-1] >= 0x08000000)
				p[-1] -= (l - 0x08000000);

			if(!argv[0])
			{
				p = (uint32 *)(0x02000000);
				for(i=0; i<256*256; i++)
					p[i] = 0;
			}

			//reset_io();

			SETW(REG_IE, 0);
			SETW(REG_IF, 0);
			SETW(REG_IME, 0);


			//SETW(REG_DISPCNT, DISP_MODE_0 | DISP_BG1_ON );
			//SETW(REG_BG1CNT, 0);

			//p = (uint32 *)0x03007FC0;
			//for(i=0; i<4*4; i++)
			//	p[i] = 0;

			SETW(REG_SOUNDBIAS, 0x0200);

#ifdef CARTLIB
			fcExecuteRom(l, jump_adress);
#else
			__FarProcedure(executeCart, jump_adress, l);
#endif

			while(1);
		}
	}
}

void execv_jump(const char *cmdname, const char *const *argv, void *jump)
{
	jump_adress = (int)jump;
	execv(cmdname, argv);
	jump_adress = 0;
}

void execv_mb(const char *cmdname, const char *const *argv)
{
	int fd = open(cmdname, 0);
	if(fd >= 0)
	{
		read(fd, (uchar *)0x02000000, 1024*256);

		SETW(REG_IE, 0);
		SETW(REG_IF, 0);
		SETW(REG_IME, 1);
		SETW(REG_DISPCNT, DISP_MODE_0 | DISP_BG1_ON );
		SETW(REG_BG1CNT, 0);
		((void(*)(void))0x02000000)();
	}

}

void device_doirq(void)
{
	int i;
	for(i=0; i<dev_count; i++)
		if(deviceList[i].irqfunc)
			deviceList[i].irqfunc();
}

int open(const char *name, int flags)
{
	int fd;
	Device *dev;
	char *cutname;

	if((dev = dev_fromname(name, &cutname)))
		if(dev->open) {
			fd = dev->open(cutname, flags);
			if(fd < 0)
				return fd;
			fdList[fdmap_count].dev = dev;
			fdList[fdmap_count].fd = fd;
			return fdmap_count++;
		}
	return -1;
}

int read(int fd, void *dest, int size)
{
	Device *dev;
	if((dev = dev_fromhandle(&fd)))
		if(dev->read)
			return dev->read(fd, dest, size);
	return -1;
}

int write(int fd, const void *dest, int size)
{
	Device *dev;
	if((dev = dev_fromhandle(&fd)))
		if(dev->write)
			return dev->write(fd, dest, size);
	return -1;
}

int close(int fd)
{
	FdMap *f;
	int oldfd = fd;
	Device *dev;
	if((dev = dev_fromhandle(&fd)))
		if(dev->close) {

			fdList[oldfd].dev = NULL;
			f = &fdList[fdmap_count-1];
			while(fdmap_count && f->dev == NULL) {
				fdmap_count--;
				f = &fdList[fdmap_count-1];
			}


			return dev->close(fd);
		}
	return -1;
}

int lseek(int fd, int offset, int origin)
{
	Device *dev;
	if((dev = dev_fromhandle(&fd)))
		if(dev->lseek)
			return dev->lseek(fd, offset, origin);
	return -1;
}

int ioctl(int fd, int request, ...)
{
	int rc = -1;
	Device *dev;
	va_list vl;
	va_start(vl, request);

	if((dev = dev_fromhandle(&fd)))
		if(dev->ioctl)
			rc = dev->ioctl(fd, request, vl);
	va_end(vl);
	return rc;
}

int stat(const char *name, struct stat *buffer)
{
	Device *dev;
	char *cutname;
	if((dev = dev_fromname(name, &cutname)))
	{
		if(dev->stat)
			return dev->stat(cutname, buffer);
	}
	return -1;
}

int remove(const char *name)
{
	Device *dev;
	char *cutname;
	if((dev = dev_fromname(name, &cutname)))
		if(dev->remove)
			return dev->remove(cutname);
	return -1;
}

int tell(int fd)
{
	return lseek(fd, 0, SEEK_CUR);
}
