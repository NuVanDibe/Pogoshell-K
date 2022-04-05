#ifndef GUIPARSER_H
#define GUIPARSER_H

void guiparser_readsymbols(char *fname);
Widget *guiparser_create(char *spec, char *rootitem);
Widget *guiparser_findwidget(char *name);

#endif
