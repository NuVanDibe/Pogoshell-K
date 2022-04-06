#ifndef SRAM_ACCESS_H
#define SRAM_ACCESS_H

void sram_read_bytes(char *sram_src, int size, void *dest);
void sram_write_bytes(char *sram_dst, int size, void *src);
void sram_memcpy(unsigned char *dest, const unsigned char *src, int len);
void sram_memmove(uchar *dest, uchar *src, int len);
void sram_memset(char *dest, int val, int len);
int sram_strcmp(const uchar *a, const uchar *b);
void sram_strcpy(char *dest, const char *src);

extern unsigned char srambuf[1024];
#define SET32(a, b) { sram_read_bytes((char *)&(b), 4, srambuf); sram_write_bytes((char *)&(a), 4, srambuf); }

#endif
