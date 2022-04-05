#include <pogo.h>

void sram_init(void);

typedef struct _OldSRamFile
{
	struct _OldSRamFile *next;
	char name[32];
	uchar flags;
	uchar user;
	uint32 length;
} __attribute__ ((packed)) OldSRamFile;


int sram_convert(void)
{
	char id[8];
	uchar *ptr = (uchar *)0x02000000;
	OldSRamFile sfile;
	OldSRamFile *f;
	int count = 0;
	int bank;
	int l,rc;
	FILE *fp;


	for(bank=1; bank<4; bank++)
	{
		set_ram_start(bank);
		memcpy8(id, (uchar *)0x0E000000, 8);
		id[7] = 0;
		//fprintf(2, "BANK %d: %s\n", bank, id);
		if(strcmp(id, "MINSF2") == 0)
		{
			memcpy8(&f, (uchar *)0x0E000008, 4);

			while(f)
			{
				//fprintf(2, "Checking %p\n", f);
				memcpy8(&sfile, f, sizeof(OldSRamFile));
				memcpy8(ptr, sfile.name, 32);
				memcpy8(&ptr[32], &sfile.length, 4);
				memcpy8(&ptr[36], &f[1], sfile.length);
				ptr += (36 + sfile.length);
				f = sfile.next;
				//fprintf(2, "Found %s (%d) in bank %d\n", sfile.name, sfile.length, bank);
				count++;
			}
		}
		else
			return 0;
	}

	if(count)
	{
		char name[40];
		ptr = (uchar *)0x02000000;

		sram_init();
		strcpy(name, "/sram/");

		while(count--)
		{
			memcpy8(&l, &ptr[32], 4);
			if(strcmp(ptr, "DUMMY") != 0)
			{
				sprintf(&name[6], ptr);
				fp = fopen(name, "wb");
				rc = fwrite(&ptr[36], 1, l, fp);
				fclose(fp);
				//fprintf(2, "Wrote %s to sram (%d)\n", ptr, rc);
			}
			ptr += (36 + l);
		}

		return 1;
	}

	return 0;
}
