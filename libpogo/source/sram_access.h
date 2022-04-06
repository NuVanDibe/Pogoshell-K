#ifndef SRAM_ACCESS_H
#define SRAM_ACCESS_H

void sram_read_bytes(const char *sram_src, int size, void *dest);
void sram_write_bytes(void *sram_dst, int size, void *src);
void sram_memcpy(void *dest, void *src, int len);
void sram_memmove(void *dest, void *src, int len);
void sram_memset(char *dest, int val, int len);
int sram_strcmp(const char *a, const char *b);
void sram_strcpy(char *dest, const char *src);

extern char srambuf[1024];
#define SET32(a, b) { sram_read_bytes((char *)&(b), 4, srambuf); sram_write_bytes((char *)&(a), 4, srambuf); }

#endif
