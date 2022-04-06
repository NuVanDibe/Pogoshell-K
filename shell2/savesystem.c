
#include <pogo.h>

#include "rle.h"
#include "text.h"
#include "settings.h"
#include "users.h"
#include "misc.h"
#include "msgbox.h"

extern int savebank;
extern tbox *MessageBox;

void statusbar_set(char *text);

//extern int sram_game_size = 64;


// Save the last game from bank 0 to a file in the sram filesystem
int savesys_savelastgame()
{
	int rc = 0, fd;
	char id[4];

	// See if we just executed a ROM and should save a savefile for it
	fd = open("/sram/.lastexec", 0);
	if(fd >= 0)
	{
		uchar usr;
		char savefile[40];
		read(fd, &usr, 1);
		read(fd, id, 3);
		read(fd, savefile, 128);
		close(fd);

		//sram_setuser(usr);

		// Check if there is anything to save
		uchar *ptr = (uchar *)0x0E00FFFF;
		set_ram_start(savebank);
		while(((uint32)ptr >= 0x0E000000) && (ptr[0] == 0)) ptr--;

		if(ptr - 0x0E000000 + 1 > 0)
		{
			int do_save = 1;

			if(settings_get(SF_ASKSAVE))
			{
				char q[64];
				sprintf(q, TEXT(WISH_SAVE), basename(savefile));
				do_save = msgbox_yesno(MessageBox, q);
			}
			
			if(do_save)
			{
				fd = open(savefile, O_CREAT);
				write(fd, "SAVE", 5);
				write(fd, &id, 3);
				rc = save_rle(fd, savebank);
				if(rc >= 0)
				{
					char q[64];
					sprintf(q, TEXT(SAVE_DONE), basename(savefile));
					statusbar_set(q);
				}
				close(fd);
			}
			else
			{
				statusbar_set(TEXT(SAVE_CANCEL));
			}
		}

		if(rc >= 0)
			remove("/sram/.lastexec");
		else
			statusbar_set(TEXT(SAVE_FAILED));
	}

	return rc;
}

void make_savename(char *dst, char *src, char *ext)
{
	char *ptr = strrchr(src, '/');
	if(!ptr)
		ptr = src;
	else
		ptr++;

	sprintf(dst, "/sram/%s", ptr);
	if(strlen(dst) > 34)
		dst[34] = 0;

	ptr = strrchr(dst, '.');
	if(ptr)
		*ptr = 0;
	strcat(dst, ext);
}


void make_saveheader(char *dest, char *rom)
{
	int fd;
	char *p;
	/* Get ID from rom to execute */
	strcpy(dest, "SAVE");
	fd = open(rom, 0);
	p = (char *)lseek(fd, 0, SEEK_MEM);
	/* Get 2byte ID and complement (checksum) */
	dest[5] = p[0xAD];
	dest[6] = p[0xAE];
	dest[7] = p[0xBD];
	close(fd);
}

/* Rom (GBA/BIN) executor */
int savesys_handleexec(char *current)
{
	int found = 0;
	int i,fd, rc = 1;
	char tmp[40];
	char tmp2[64];
	//char id[8];
	char *p;

	make_saveheader(tmp2, current);
	make_savename(tmp, current, ".sav");

	// Try to open a savefile with the same name as rom
	if((fd = open(tmp, 0)) < 0)
	{
		/* Unreliable.  Commenting out.
		struct dirent *de;
		DIR *d;

		// If no name-match, check all files in sram for a matching savefile
		d = opendir("/sram");
		if(d)
		{
			p = strrchr(tmp, '/');
			p++;
			while(!found && (de = readdir(d)))
			{
				sprintf(p, "%s", de->d_name);
				fd = open(tmp, 0);
				if(fd >= 0)
				{
					read(fd, id, 8);
					if(memcmp(id, tmp2, 8) == 0)
					{
						found = 1;
						close(fd);
					}
				}
			}
			closedir(d);
		}
		*/
	}
	else
	{
		close(fd);
		found = 1;
	}

	// Save the last_exec file with info on the savefile
	remove("/sram/.lastexec"); // Just to make sure
	fd = open("/sram/.lastexec", O_CREAT);
	if(fd >= 0)
	{
		uchar usr = CurrentUser;
		if(!found)
			make_savename(tmp, current, ".sav");
		write(fd, &usr, 1);
		write(fd, tmp2+5, 3);
		write(fd, tmp, strlen(tmp)+1);
		close(fd);
	}

	// Clear bank used for save
	set_ram_start(savebank);
	p = (char *)0x0E000000;
	i = 64*1024;
	while(i--)
		*p++ = 0;
	/* Load the save file */
	if(found && rc)
	{
		int do_load = 1;
		if(settings_get(SF_ASKLOAD))
		{
			char q[64];
			sprintf(q, TEXT(WISH_LOAD), basename(tmp));
			do_load = msgbox_yesno(MessageBox, q);
		}

		if(do_load)
		{
			fd = open(tmp, 0);
			read(fd, tmp2, 8);
			if(!(tmp2[0] == 'S' && tmp2[1] == 'A' && tmp2[2] == 'V' && tmp2[3] == 'E'))
				lseek(fd, 0, SEEK_SET);
			load_rle(fd, savebank);
			close(fd);
		}
	}

	return 1;
}
