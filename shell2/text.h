#ifndef TEXT_H
#define TEXT_H

#define TEXT(i) text_strings[i]

enum {

WISH_LOAD,
NEW_FONT,
YES,
NO,
WISH_SAVE,
SAVE_CANCEL,
SAVE_DONE,
SAVE_FAILED,
SRAM_HELP_TITLE,
SRAM_HELP_TEXT,
SETTINGS_HELP_TITLE,
SETTINGS_HELP_TEXT,
MAIN_HELP_TITLE,
MAIN_HELP_TEXT,
SRAM_COPY,
SRAM_PASTE,
SRAM_DEL,
RAM_RENAME,
QUESTION,
TREAD_HELP_TITLE,
TREAD_HELP_TEXT,
TITLEBAR_MAIN,
TITLEBAR_SRAM,
INTRO_LINE,
LOADING_LIST,
PLEASE_WAIT,
INVALID_PLUGIN,
WISH_SRAM_DEL,
SWITCHED_USER,
CMD_BASIC,
CMD_SRAM,
COMMANDS,

SETTINGS_START,

TEXT_END = SETTINGS_START + 11,

};

extern char *text_strings[TEXT_END];

int read_texts(FILE *fp);

#endif
