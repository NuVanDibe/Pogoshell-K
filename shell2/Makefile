
### Location of libpogo
POGO=../libpogo

ifeq ($(CART),)
#error "You need to define CART to your cart type!"
endif

ifeq ($(EXT),)
EXT=gba
endif

### Location of flashcart library to use
LIBFCLASHCART = ../libfc_$(CART)/libflashcart.a


LIBFILE = libpogo.a
LIBS = -L$(POGO)/lib -lpogo -lgcc

#ifeq (mb, $(EXT))
#DEFINES += MULTIBOOT
#endif

CFLAGS	= $(DEFINES) -Wall -c -fomit-frame-pointer -fno-builtin -mthumb -mthumb-interwork -Wall -g -Os -nostdinc -I$(POGO)/include
CFLAGS2 = $(DEFINES) -Wall -c -fomit-frame-pointer -fno-builtin -mthumb -mthumb-interwork -Wall -g -Os -nostdinc -I$(POGO)/include
CFLAGS3 = $(DEFINES) -Wall -c -fomit-frame-pointer -fno-builtin -mthumb -mthumb-interwork -Wall -g -Os -nostdinc -I$(POGO)/include
CFLAGS4 = $(DEFINES) -Wall -c -fomit-frame-pointer -fno-builtin -mthumb -mthumb-interwork -Wall -g -O3 -nostdinc -I$(POGO)/include


ASFLAGS = -mthumb-interwork

PREFIX=arm-elf-
CC = $(PREFIX)gcc
LD = $(PREFIX)gcc
AS = $(PREFIX)as
LDFLAGS = -Tlnkscript
OBJCOPY = $(PREFIX)objcopy

TARGET = pogo

WIDGETS = widgets/textbar.o widgets/tricontainer.o widgets/listview.o widgets/textflow.o widgets/scrollbar.o widgets/typeface.o

CART_OBJFILES = main.o filetype.o filesys.o

FULL_OBJFILES = bmview.o bmpview.o jpgview.o jpeg.o iwram.o sram_convert.o widgets/textreader.o

COMMON_OBJFILES = crt0.o text.o unapack.o bitmap.o window.o backdrop.o \
	   guiparser.o savesystem.o rle.o msgbox.o users.o  \
		   misc.o gameromfs.o settings.o syscall.o $(WIDGETS)

OBJFILES = $(COMMON_OBJFILES) $(FULL_OBJFILES) $(addprefix $(CART)_obj/, $(CART_OBJFILES))


all : $(CART)_obj $(TARGET)_$(CART).$(EXT) 

$(CART)_obj:
	mkdir -p $(CART)_obj		

../libfc_$(CART)/libflashcart.a :
	make -C ../libfc_$(CART)

mb :
	mb -s $(TARGET)_$(CART).$(EXT) -c -w 250 -x 400

unapack.o: unapack.c
	$(CC) -c $(CFLAGS4) $< -o $@

iwram.o : iwram.c
	$(CC) -c $(CFLAGS3) $< -o $@

users.o : users.c
	$(CC) -c $(CFLAGS2) $< -o $@

$(CART)_obj/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.S
	$(AS) $(ASFLAGS) $< -o $@

clean :
	rm -f *.o widgets/*.o *.gba *.elf *.sav *.plg *.ncb *.bak *_obj/*.o ../libfc_$(CART)/*.o ../libfc_$(CART)/*.a

rebuild: clean all

$(TARGET)_$(CART).$(EXT) : $(TARGET)_$(CART).elf Makefile
	$(OBJCOPY) -O binary $(TARGET)_$(CART).elf $(TARGET)_$(CART).$(EXT)
	gbafix -t"POGOSHELL" $(TARGET)_$(CART).$(EXT)

$(TARGET)_$(CART).elf : $(OBJFILES) $(POGO)/lib/$(LIBFILE) $(LIBFCLASHCART)
	$(LD) -mthumb-interwork -nostdlib -Wl,$(LDFLAGS) -o $(TARGET)_$(CART).elf $(OBJFILES) $(LIBS) $(LIBFCLASHCART)
