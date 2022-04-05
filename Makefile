#
# Top Makefile for pogo
#

DISTRO = PogoShell2.0Beta3


all : libpogo tools flashlib pogoshell
.PHONY : libpogo tools pogoshell flashlib clean


distro:
	rm -rf $(DISTRO)
	mkdir -p $(DISTRO)
	cp -a data/root $(DISTRO)
	cp -a data/distro/* $(DISTRO)
	mkdir -p $(DISTRO)/tools
	cp -a tools/*.exe $(DISTRO)/tools
	cp shell2/pogo.gba $(DISTRO)

libpogo: 
	make -C libpogo/source all
	make -C libpogo/source -f Makefile.deb all

tools:
	# make -C tools
	make -C data/fonts

pogoshell:
	make -C shell2 CART=xrom
#	make -C shell2 CART=f2axg
	make -C shell2 CART=visoly_xg1
#	make -C shell2 CART=ez

clean:
	make -C libpogo/source clean
	make -C libpogo/source -f Makefile.deb clean
	make -C data/fonts clean
	make -C shell2 clean
	make -C libfc_visoly_xg1 clean
	make -C libfc_xrom clean
	
superclean: clean
	make -C tools clean
	rm -rf $(DISTRO)

rebuild : clean all
