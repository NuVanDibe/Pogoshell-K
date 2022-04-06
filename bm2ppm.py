
import sys, os
from sys import argv
from struct import pack, unpack

def readfile(name):
	fd = open(name, "rb")
	if not fd:
		print "Error reading", name
	contents = fd.read()
	fd.close()
	return contents

def toRGB(pixel):
	color = unpack("<h", pixel)[0]
	red = ((color>>0)&31)<<3
	green = ((color>>5)&31)<<3
	blue = ((color>>10)&31)<<3
	return chr(red) + chr(green) + chr(blue)

if __name__ == "__main__":
	if len(argv) == 3:
		contents = readfile(argv[1])
		fd = open(argv[2], "wb")
		fd.write("P6\n")
		depth, width, height = unpack("<3h", contents[2:8])
		data = contents[8:]
		fd.write("%d\n%d\n255\n" % (width, height))
		for y in range(height):
			for x in range(width):
				pixel = data[y*width*2+x*2] + data[y*width*2+x*2+1]
				fd.write(toRGB(pixel))
		fd.close()
	else:
		print "Usage: %s file.bm file.ppm" % argv[0]
