#
# libpogo Makefile
# (C) 2002 - 2004 Jonas Minnberg

ifeq ($(DEBUG),1)
CFLAGS	= -DDEBUG -Wall -c -g  -fno-builtin -mthumb -mthumb-interwork -I. -I../include
LIBNAME = libpogod.a
OBJDIR = objd
else
CFLAGS	= -Wall -Os -fno-builtin -fomit-frame-pointer -mthumb -mthumb-interwork -I. -I../include
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

COREFILES = memory.o core_misc.o core.o device.o font.o rtc.o
ASMFILES = sema.o dual.o
DEVICES = screen.o keyboard.o vkeyboard.o smartkey.o editbuf.o cursor.o \
          console.o romfilesys.o sramfile.o sram_access.o debug.o
LIBFILES = string.o io.o misc.o


OBJFILES = $(addprefix $(OBJDIR)/,$(LIBFILES) $(COREFILES) $(DEVICES) $(ASMFILES))


$(OBJDIR)/old_%.o : %.c
	$(CC) -c $(CFLAGS) -DOLD_VERSION $< -o $@


$(OBJDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : %.s
	$(AS) $(ASFLAGS) $< -o $@


all: ../lib/$(LIBNAME)

../lib/$(LIBNAME) : $(OBJFILES)
	$(AR) rs $@ $(OBJFILES)

clean :
	rm -f $(OBJFILES) ../lib/$(LIBNAME) *.bak *.opt *.ncb

rebuild: clean all
