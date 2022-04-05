#ifndef MSGBOX_H
#define MSGBOX_H

int msgbox_yesno(char *text);
int msgbox_list(char *title, char **lines, int num);
void msgbox_info(char *title, char *text);
int msgbox_list2(char *title, char *str, int num);

#endif
