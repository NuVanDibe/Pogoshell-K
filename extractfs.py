
import sys, os
from sys import argv
from stat import *
from struct import unpack

ROMFILESIZE = 40

def make_valid(name):
	new_name = ""
	for f in name:
		if ord(f) == 0:
			return new_name
		elif ord(f) < 32 or ord(f) > 127 or f == '/' or f == '\\':
			new_name += "_"
		elif sys.platform == "win32" and \
			(f == ':' or f == '?' or f == '*' or f == '"'):
			new_name += "_"
		else:
			new_name += f
	return new_name

def readfile(name):
	fd = open(name, "rb")
	contents = fd.read()
	fd.close()
	return os.stat(name)[ST_SIZE], contents

def process(contents, position, length, rootstart):
	for i in range(0,length,ROMFILESIZE):
		(name, size, start) = unpack("<32sIi", contents[position+i:position+i+ROMFILESIZE])
		name = make_valid(name)
		start += rootstart
		if size & 0x80000000:
			os.mkdir(name)
			os.chdir(name)
			process(contents, start, size & 0x7fffffff, rootstart)
			os.chdir("..")
		else:
			fd = open(name, "wb")
			fd.write(contents[start:start+size])
			fd.close()

if __name__ == "__main__":
	if len(argv) == 2:
		pos = 0
		size, contents = readfile(argv[1])
		if size < 4:
			print "File is too short to be a Pogoshell image."
		else:
			magic = unpack("<I", contents[pos:pos+4])[0]
			while pos + 4 < size and magic != 0xfab0babe:
				pos += 32768
				magic = unpack("<I", contents[pos:pos+4])[0]
			if magic != 0xfab0babe:
				print "This isn't a Pogoshell image."
			else:
				process(contents, pos+8, unpack("<I", contents[pos+4:pos+8])[0] & 0x7fffffff, pos+8)
	else:
		print "Usage: %s flashme.gba" % argv[0]
