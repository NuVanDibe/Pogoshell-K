
import sys, os, re
from sys import argv
from struct import pack, unpack

def readfile(name):
	fd = open(name, "rb")
	if not fd:
		print "Error reading", name
	contents = fd.read()
	fd.close()
	return contents

def toPixel(red, green, blue, transparent = 0):
	transparent <<= 15
	red = ((red>>3)&31)<<0
	green = ((green>>3)&31)<<5
	blue = ((blue>>3)&31)<<10
	return pack("<h", transparent + red + green + blue)

if __name__ == "__main__":
	if len(argv) == 3 or len(argv) == 4:
		contents = readfile(argv[1])
		fd = open(argv[2], "wb")
		fd.write("BM")
		i = 0
		while i < len(contents):
			if contents[i] == "#":
				while i < len(contents) and contents[i] != '\n':
					i += 1
				i += 1
				continue
			break
		if contents[i:i+2] != "P6":
			print "Error: Unsorted file."
			sys.exit(1)
		i += 3
		while i < len(contents):
			if contents[i] == "#":
				while i < len(contents) and contents[i] != '\n':
					i += 1
				i += 1
				continue
			break
		while contents[i] in " \t\r\n":
			i += 1
		i1 = i
		while contents[i] not in " \t\r\n":
			i += 1
		i += 1
		width = int(contents[i1:i])
		print width
		while i < len(contents):
			if contents[i] == "#":
				while i < len(contents) and contents[i] != '\n':
					i += 1
				i += 1
				continue
			break
		while contents[i] in " \t\r\n":
			i += 1
		i1 = i
		while contents[i] not in " \t\r\n":
			i += 1
		i += 1
		height = int(contents[i1:i])
		print height
		while i < len(contents):
			if contents[i] == "#":
				while i < len(contents) and contents[i] != '\n':
					i += 1
				i += 1
				continue
			break
		while contents[i] in " \t\r\n":
			i += 1
		i1 =i
		while contents[i] not in " \t\r\n":
			i += 1
		i += 1
		maxcolors = int(contents[i1:i])
		print maxcolors
		data = contents[i:]
		if maxcolors < 256:
			depth = 16
		else:
			print "Error: Unsupported file type."
			sys.exit(1)
		fd.write(pack("<3h", depth, width, height))
		if len(argv) == 3:
			tred, tgreen, tblue = -1, -1, -1
		else:
			tred, tgreen, tblue = ord(data[0]), ord(data[1]), ord(data[2])
		for y in range(height):
			for x in range(width):
				red = ord(data[y*width*3+x*3])
				green = ord(data[y*width*3+x*3+1])
				blue = ord(data[y*width*3+x*3+2])
				if tred == red and tgreen == green and tblue == blue:
					pixel = toPixel(red, green, blue, 1)
				else:
					pixel = toPixel(red, green, blue)
				fd.write(pixel)
		fd.close()
	else:
		print "Usage: %s file.ppm file.bm [enable_transparent]" % argv[0]
