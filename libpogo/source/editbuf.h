#ifndef EDITBUF_H
#define EDITBUF_H

int editbuf_read(char *dest, int count);
void editbuf_init(char *mem, int size);
void editbuf_put(int c);
void editbuf_write(char *s, int count);
char *editbuf_get(void);
int editbuf_del(int start, int count);
int editbuf_commit(void);
int editbuf_seek(int offset, int where);
int editbuf_changed(void);
void editbuf_set(char *s);

#endif
