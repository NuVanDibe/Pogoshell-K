# Make all fonts
#

MF = ../../tools/makefont128.exe

all: nokia.font screenfox.font fixed6.font fixed5.font fixed4.font fixed4.font

nokia.font : nokia.bmp
	$(MF) nokia.bmp nokia.font -5 348

screenfox.font : screenfox.bmp
	$(MF) screenfox.bmp screenfox.font -5 -49 -51

fixed6.font : fixed6.bmp
	$(MF) fixed6.bmp fixed6.font -f

fixed5.font : fixed5.bmp
	$(MF) fixed5.bmp fixed5.font -f

fixed4.font : fixed4.bmp
	$(MF) fixed4.bmp fixed4.font -f -e122

clean:
	rm -f *.font *.log
