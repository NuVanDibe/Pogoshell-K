
all: makefs.exe html2pogo.exe linkwrap.exe makefont.exe

makefs.exe : makefs.c
	cl -DWIN32 makefs.c
	rm -f makefs.obj

html2pogo.exe : html2pogo.c
	cl html2pogo.c
	rm -f html2pogo.obj

linkwrap.exe : linkwrap.c
	cl linkwrap.c
	rm -f linkwrap.obj

makefont128.exe : makefont.c bmp.c
	cl makefont.c bmp.c
	rm -f makefont.obj bmp.obj

clean :
	rm -f *.obj *.exe

rebuild: clean all
