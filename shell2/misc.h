#ifndef MISC_H
#define MISC_H

enum { LZ77 = 1, APACK = 2, RAW = 0 };

void reset_gba (void);
char *basename(char *str);
void *pmalloc(int size);
int pmemory_free(void);
void *pmemory_pointer(void);
void pfree(void);
int read_line(char *line, int size, FILE *fp);
int find_section(FILE *fp, char *name);
//int file2ram(char *fname, void *mem, int msize);
uchar *file2mem(char *fname, void *mem, int msize, int decompression);
int parse_assignment(char *line, char **name, char **val);
Font *font_load_path(char *name);
char *strdup(char *str);
extern char *path[5];
enum {PATH_PLUGINS, PATH_THEMES, PATH_FONTS, PATH_BITMAPS, PATH_SCREENSAVERS};
#define GET_PATH(x) (path[PATH_ ## x])


#endif
