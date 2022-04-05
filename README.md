# Pogoshell-K
A long while ago, a GBATemp member named Kuwanger picked up work on a then-abandoned project called Pogoshell, by sasq. After some great improvements, development once again stalled.

Tragically, Kuwanger passed away in 2019, and his site went down. [Thankfully it was crawled by Internet Archive's WayBack Machine](https://web.archive.org/web/20150430211129/http://kuwanger.net/), so we can go back and pick up the source.

This will be a slow project to evolve and it's possible that no further improvements to Pogoshell may ever be made. But, feel free to fork as you wish.

Below, you can find Kuwanger's original readme from [the archive of his website.](https://web.archive.org/web/20150430211127/http://kuwanger.net/gba/pogoshell/)

# Pogoshell
Pogoshell is a file manager for the Gameboy Advance. Really, that's an oversimplification. Pogoshell also includes a much better save management system than most flash cart frontends provide. Another big feature is plug-ins, a system of loading files through an association system. Finally, Pogoshell supports a theming system, offering a much more customable interface.

With the development of Pogoshell v2.0b4, official development of Pogoshell has stalled. While there was a Subversion archive setup with work towards, assumedly, a v2.0b5, the work was never completed towards an official release and doesn't properly work when simply compiled. With v2.0b4's source unavailable, I decided to continue work using v2.0b3's source. So far, this has meant merging in some changes from the Subversion archive, changing theme support in many ways, and removing a few limitations to things like file listings. For a more complete list of new features, look below. Note, internal mod player support had to be removed, as the license for it required permission to redistribute, and I personally didn't want to require others to ask permission to make their own forks.

## Features (above Pogoshell v2.0b3)
- Maximum of 500 files per directory
- Gameboy Bridge Support
- Internal JPEG viewer
- Internal encrypted JPEG viewer
- Internal apack compressed multiboot support
- Hidden directory support*
- Theme Switching support
- Improved Theming options*
- More text**
- Screeensaver support
- Limited list position tracking between directories/games
- Fixes some close() bugs
- Easier list movement with left/right
- Font truncation (to avoid writing writing outside of a widget)
- And others I'm forgetting

*Features incompatible with either Pogoshell v2.0b3 tools or with v2.0b3 themes
**Always be sure that you update your pogo.cfg on each new release; the texts and symbols section may have changed

## Theming
My unofficial Pogoshell fork includes several new features to improve the ability to theme your Pogoshell how you want as well as fix a few quirks that existed in v2.0b3. Major changes include converting the list scrollbar to an actual scrollbar widget. Also included is the ability to make the list highlight bar semi-transparent (as present in v2.0b4 and later), as well as adding or subtracting a fixed color. It is possible to use gradients and bitmaps in all lists now, without weird redraw problems. And it is possible to align each column in a list as one sees fit. Finally of interest is more attributes for the TEXTFLOW mflow are copied over for use in message box lists.

For a mostly complete to theming, look here: [Unofficial Pogoshell Theming](https://web.archive.org/web/20150430211139/http://kuwanger.net/gba/pogoshell/theming.shtml). If the information provided is still not helpful, feel free to look at the theme(s) available for my unofficial Pogoshell fork. They may be helpful.

# Gameboy Bridge Support
The Gameboy Bridge is an interesting device allowing a Flash2Advance cart to behave like a Gameboy [Color] cart. Before Goomba, this was the only way to play Gameboy games on a Gameboy Advance with a Flash2Advance. Now, with Goomba [Color], the Gameboy Bridge has been mostly forgotten. It still is useful, however, for those games that Goomba [Color] doesn't properly support.

To use the Gameboy Bridge properly, you'll need to do a few things. First, you will need to edit /.shell/pogo.cfg. Change SRAM=192 to SRAM=128 (to allocate 64KB for all the Gameboy saves) and SAVEBANK=0 to SAVEBANK=1 (so Pogoshell will start at bank 1 for GBA SRAM and its own filesystem)--if you've already used Pogoshell and have saves, be sure to fix the Pogoshell filesystem by making sure you have 64KB free and using [SramExplorer v2.0b4](https://www.zophar.net/utilities/gbautil/sram-explorer.html) to shrink the SRAM size from 192 to 128. You'll also want to add a line in the filetypes section that reads something like "gbx 2 GBX".

Now, create a folder in your Pogoshell root (I use .gbx so it's hidden by default). Create a copy of GBC2GBA.GB in that folder along with all the Gameboy [Color] games you want. Rename all the Gameboy [Color] games to have a gbx extension (eg game.gbx).

Finally, you're going to have to make some consideration about the Gameboy [Color] games saves. The Gameboy Bridge allocates for the first 8 games 8KB each for saves (ie up to 64KB total). If a game actually requires 32KB of saves, you'll need to put 3 games (ie 24KB) of space between it and the next games that saves. To faciliate this, makefs.py will sort how games are added based on their filename. So, you can manually sort games to your liking by renaming them appropriate (eg "largesave.gbx" can become "00 largesave.gbx", "nosave1.gbx" can become "01 nosave1.gbx", etc). Unless you're trying to use a game with a large save, have a lot of (ie over 8) Gameboy [Color] games you want to use with the Gameboy Bridge, or just want to keep the order static in case you add games later, all of this is probably a non-issue for you.

Having set this all up, to create an appropriate flashme.gba with makefs.py, include either the "-b" or "-h" option (eg "makefs.py -b pogo_visoly_xg1.gba root root.gba"). The "-b" option will cause makefs.py to make a proper build of flashme.gba. The "-h" option will do what the "-b" option does along with hiding gbc2gba.gb and all the ".gbx" files. Providing both options is equivalent to applying the "-h" option.

# Screen Savers
Screen savers are similar to what they are like on Windows. They are of a slight difference however. Specifically, the SCREENSAVERS directory in pogo.cfg is used as a source for files or programs to be used as screen savers. After the specified amount of time while being idle (configurable under settings), one of the screen savers is chosen randomly to be used. It continues to be ran until exited as usual. If either screen saver support has been disabled or the SCREENSAVERS directory is empty, after the specified time the GBA will go into suspend mode and one needs to use START+SELECT to wake Pogoshell up.

# Hidden Directories
Hidden directories are a means of hiding from casual view entire directories of files. Combined with encrypted JPEGs and the use of more secure padding, they're extremely difficult to discover. To use, one first must deciding on where the hidden directory will reside. It may not exist as a subdirectory of another hidden directory. Nor may it be used to store system files (ie, plugins, themes, etc) nor may multiple hidden directories exist in the same directory. Having decided where to store this directory, choose a combination of many up, down, left, and right moves (the Konami code is probably a bad choice, given how well known it is). Now, use the included keydir.py and enter this code as u, d, l, and r. The longer the code, obviously the harder it will for someone to randomly guess. If provided with the -m option, keydir.py will make the directory for you. Otherwise, it will give you the directory name to use should you decide to create it.

With the directory created some choices should be made. First, if this purely to prevent casual snooping, feel free to put any file you like with it. The directory listing itself is limited to 64 files. This is because the directory listing is encrypted. For less casual snooping, it is advised to only use encrypted files or those without discernable headers. This is because while the directory listing is encrypted, files inside of it are not (obviously, unless you encrypted them yourselves..and then you'd have to have something to understand encrypted files). As such, someone who would dump the cart would be able to see many files placed throughout the cart which they could access. Also, be advised to not include directories within hidden directories. This is mainly ill-advised because subdirectories aren't encrypted either, and such would show that a hidden directory does exist somewhere. Also, if they're able to dump your cart, they may have access to your computer. So, obviously keep your Pogoshell root stored in some form of encrypted volume (EncFS won't suffice, as the filename used for hidden dirs, $key$..., is obvious).

Within Pogoshell, entering a hidden directory is simple. First, enter the directory containing the hidden directory. Then, press the correct order of directions. If done correctly, the last direction pressed will cause you to enter the hidden directory. If you make a mistake while entering the code in the containing directory, exit and reenter the directory. Hopefully you'll find interesting uses for hidden directories.

# JPEGs and Encrypted JPEGs
My Pogoshell includes a jpeg viewer. It is based off gba-jpeg by .... It is able to display images of ~400x300 size (specifically, those images that have a decompressed size of 252KB or less; ie ~129024 pixels). This major limitation is mostly because I haven't worked out a good method for on-the-fly decompression yet. It is also why loading of images can take upwards of 2-3 seconds.

Encrypted JPEGs are a related beast. My own creation, they are images that have been encrypted with jpe.py that allow up, down, left, right, L, R, A, and B, to be used to generate a 128-bit key for AES to use for encryption. Use jpe.py with your combination, using u, d, l, r, L, R, a, b to specify the buttons for your key. Pressing START after entering your key in the JPEG viewer for decryption. Some validation is attempted to verify that the decrypted file is an actual JPEG, and then it is displayed; an invalid key may hang the GBA. Every time a key is entered, it is saved in RAM for further use. One can use SELECT to load the last key used at any time. Because of this, it is advised to blank the key (by running a rom, a plugin, or entering an invalid key) if you're away from the GBA for a while.

Because encrypted JPEGs are decrypted, the available amount of space for them is further limited. This limitation is relative to the file size. In general, one can use the formula (jpeg_width*jpeg_height*2+encrypted_jpeg_size) < 252KB to test the truth value of whether the image will display correctly. You should find that most jpegs of ~400x300 should be small enough (that'd leave enough room for a 17.5KB JPEG). Feel free to tweak the quality setting in your jpeg editor of choice, as a combination of the TFT LCD screen and small size makes it much more difficult to see artifacts.

Use of the JPEG viewer is also pretty nice. Use left/right/up/down to scroll through the image. Hold down the L trigger to scroll faster. Use the R trigger plus left/right to rotate the image by 90 degree increments. Use the R trigger plus up/down to change the scale factor. One can also use the SELECT button to toggle through aspect ratio scale, stretch scaling, and back to manual scale selection. Use R trigger plus START to toggle anti-aliasing; this is slower but makes scaling less than 100% look better. Use START alone to reset manual scaling to 100%; you'll still need to use SELECT to switch to manual scale mode. Finally, A and B can both be used to exit.
