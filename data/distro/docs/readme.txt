 POGOSHELL 2.0 BETA 3
----------------------
 Released 8 Mars 2004


This is another public release of Pogoshell 2.0 beta
There's no real documentation yet, feel free to experiment :)

If you find bugs you think I am not aware of you can post on
the forum or even mail me.

CARTS FULLY SUPPORTED

o Visoly/F2A
o XG1
o XRom

Only the first 256MBit of a cart can be used by PogoShell!

-

NEWS SINCE BETA 2

o XRom support
o Source release
o Small fixes

NOTE TO Xrom users!

Replace the files in the root PogoShell folder with the ones
found in xrom/ to use the xrom version of pogoshell.



NEWS SINCE BETA 1

* Old SRAM filesystem will be converted to new automatically
  first time you run pogoshell. I suggest you test this in
  the emulator first

* More compatible start of roms. Reset Start is back but
  shouldn't be needed.

* New configuration layout. pogo.cfg contains all configuration
  and points out all other files & directories. Look through it.

* Settable Size of SRAM. The SRAM=<size> parameter in pogo.cfg
  decides how much of availbale SRAM should be used for the pogo
  sram filesystem. This is the upper banks, the lower banks are
  then left untouched. For now, you should really only need to
  change that for using the gb-bridge I guess.

* Icon width & height are changeable

* Colors of marked line is settable

* Some minor fixes that I've forgotten about.



Again note - this is still a BETA. A final version will be released
when I feel it is stable enough and the final features are in.


USING

Run "BUILD.BAT" to build a rom from your "root" directory
Now you can either:

* Run "TEST.BAT" to test your rom in Visual Boy Advance
* Flash "flashme.gba" to your cart
* Flash "pogo.gba" and "filesys.bin" separatly to your
  cart with any flash program - as long as "filesys.bin"
  is after "pogo.gba" the files will be found. If you
  put normal roms in between they should also be found
  by pogoshell

(NOTE: If you run the emulator with a gba bios file it
 will work even better; for instance, nfs-playing will work)

USERS

You dont have to bother with users, but L+SELECT lets you
choose between the users defined in .shell/users

Different users will not see each others sram-saves, but you
can use Copy/Paste to copy a file from one user to another.

User 100 is the textreader and all generated indexes are
saved as that user.


THEMES

Right now, ".shell/default.theme" is loaded - rename one of the
others to use that. The themes basically define the GUI
components - feel free to experiment.

The tool bmp2bm is used to create icons and backround bitmaps.
It's extremely primitive at the moment, just bmp2bm <infile> <outfile>
and pure green becomes transparent.

Also note that the theme-file actually defines the whole GUI so you
can't change it any way you want - the basic components must be there.



MAIN CHANGES SINCE v1.3

* Unified SRAM (one big bank) 
* Working XG1 support
* Completely themable (new GUI system) with background graphics
  and scrollbars 
* Color fonts 
* 16bit graphics 
* Internal BMP viewer 
* Simple multiuser (to have multiple saves for one game) 
* SRAM can be used for normal files (texts, mods etc) 
* Better support for mixing Pogoshell with other menu, and
  changing games without reflashing everything 
* Pogoshell detects roms not in the pogo filesystem but in the
  cart and puts them in a special folder 
* Multiboot as plugin - goomba & pocketnes runs as multiboot which
  means they take less space and works on any cart and emulator 
* Settable per plugin if savename should be taken from plugin name
  or file (rom) name 
* Preliminary RTC Support 


-- Sasq
(jonas_minnberg@yahoo.se)
