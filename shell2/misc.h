#ifndef MISC_H
#define MISC_H

char *basename(char *str);
int read_line(char *line, int size, FILE *fp);
int find_section(FILE *fp, char *name);
int file2ram(char *fname, void *mem, int msize);
uchar *file2mem(char *fname, void *mem, int msize);
int parse_assignment(char *line, char **name, char **val);
Font *font_load_path(char *name);
char *strdup(char *str);
extern char *path[4];
enum {PATH_PLUGINS, PATH_THEMES, PATH_FONTS, PATH_BITMAPS};
#define GET_PATH(x) (path[PATH_ ## x])


#endif
