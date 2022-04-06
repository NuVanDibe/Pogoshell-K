#
# libpogo Makefile
# (C) 2002 - 2004 Jonas Minnberg

ifeq ($(DEBUG),1)
CFLAGS	= -DDEBUG -Wall -c -g  -fno-builtin -mthumb -mthumb-interwork -I. -I../include
FONT_CFLAGS	= -DDEBUG -Wall -c -g  -fno-builtin -mthumb -mthumb-interwork -I. -I../include
LIBNAME = libpogod.a
OBJDIR = objd
else
CFLAGS	= -Wall -Os -fno-builtin -fomit-frame-pointer -mthumb -mthumb-interwork -I. -I../include
FONT_CFLAGS	= -Wall -Os -fno-builtin -fomit-frame-pointer -mthumb -mthumb-interwork -I. -I../include
LIBNAME = libpogo.a
OBJDIR = obj
endif

ASFLAGS = -mthumb-interwork

PREFIX = arm-elf-
AS = $(PREFIX)as
CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
AR = $(PREFIX)ar
OBJCOPY = $(PREFIX)objcopy

COREFILES = memory.o core_iwram.o core_misc.o core.o device.o font.o rtc.o
ASMFILES = sema.o
DEVICES = screen.o keyboard.o vkeyboard.o smartkey.o editbuf.o cursor.o \
          console.o romfilesys.o sramfile.o sram_access.o debug.o
LIBFILES = string.o io.o misc.o aes.o


OBJFILES = $(addprefix $(OBJDIR)/,$(LIBFILES) $(COREFILES) $(DEVICES) $(ASMFILES))

all: ../lib/$(LIBNAME)


$(OBJDIR)/old_%.o : %.c
	$(CC) -c $(CFLAGS) -DOLD_VERSION $< -o $@


$(OBJDIR)/font.o : font.c
	$(CC) -c $(FONT_CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@

../lib/$(LIBNAME) : $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

clean :
	rm -f $(OBJFILES) ../lib/$(LIBNAME) *.bak *.opt *.ncb

rebuild: clean all
