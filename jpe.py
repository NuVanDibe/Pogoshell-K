"""
jpe.py

Encrypts jpeg for use by internal jpg viewer in
Unofficial Pogoshell v2.0b3-mod5.  Add "jpe 6 JPE"
or equivalent to pogo.cfg to use.

Uses rijndael.py from the tlslite package

Usage: jpe.py file.jpg file.jpe

Made by Kuwanger in Jan. 2006
"""

import sys, os
from random import SystemRandom
from rijndael import rijndael
from sys import argv
from stat import *
from struct import pack

def readfile(name):
	fd = open(name, "rb")
	if not fd:
		print "Error reading", name
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
		pad = (16-(size%16))%16
		if pad:
			contents += long2chrl(SystemRandom(0).getrandbits(pad*8), pad)
		fd.write(pack("<i", size))
		salt = SystemRandom(0).getrandbits(128)
		'''salt = 0xfedcba9876543210fedcba9876543210'''
		fd.write(long2chrl(salt, 16))
		key = getkey(salt)
		size += pad
		r = rijndael(long2chrl(key, 16), block_size = 16)
		new = ""
		enc = long2chrl(0xfedcba9876543210fedcba9876543210, 16)
		for i in range(0,size,16):
			temp = xorstr(contents[i:i+16], enc)
			enc = r.encrypt(temp)
			new += enc
		fd.write(new)
		fd.close()
	else:
		print "Usage: %s file.jpg file.jpe" % argv[0]
