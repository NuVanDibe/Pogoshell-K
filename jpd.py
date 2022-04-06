"""
jpd.py

Encrypted jpeg decryptor.
Uses rijndael.py from the tlslite package

Usage: jpd.py file.jpe file.jpg

Made by Kuwanger in Jan. 2006
"""

import sys, os
from rijndael import rijndael
from sys import argv
from stat import *
from struct import unpack

def readfile(name):
	fd = open(name, "rb")
	contents = fd.read()
	fd.close()
	return os.stat(name)[ST_SIZE], contents

def xorstr(str, enc):
	xstr = ""
	for i in range(len(str)):
		xstr += chr(ord(str[i])^ord(enc[i]))
	return xstr

def long2chrl(lng, len):
	ret = ""
	for i in range(0,len*8,8):
		ret = ret + chr((lng>>i)&0xff)
	return ret

def chr2longb(str):
	lng = 0
	for i in range(len(str)-1,-1,-1):
		lng = (lng<<8) + ord(str[i])
	return lng

def chr2longl(str):
	lng = 0
	for i in str:
		lng = (lng<<8) + ord(i)
	return lng

def getkey(salt):
	d = { 'u': 0, 'd': 1, 'l': 2, 'r': 3, 'L': 4, 'R': 5, 'a': 6, 'b': 7 }
	key = salt
	input = raw_input("Key: ")
	for f in input:
		if d.has_key(f):
			key = ((key << 3) | d[f]) & ((1 << 128) - 1)
		else:
			print "Invalid input:", f
			print "Valid input is u, d, l, r, L, R, b, a"
			sys.exit(1)
	return key

if __name__ == "__main__":
	if len(argv) == 3:
		fd = open(argv[2], "wb")
		size, contents = readfile(argv[1])
		realsize = unpack("<i", contents[0:4])[0]
		salt = contents[4:20]
		key = getkey(chr2longb(salt))
		r = rijndael(long2chrl(key, 16), block_size = 16)
		old = ""
		old_enc = long2chrl(0xfedcba9876543210fedcba9876543210, 16)
		for i in range(20,size,16):
			enc = contents[i:i+16]
			plain = xorstr(r.decrypt(enc), old_enc)
			old += plain
			old_enc = enc
		fd.write(old[0:realsize])
		fd.close()
	else:
		print "Usage: %s file.jpe file.jpg" % argv[0]
