
import sys, os, getopt
from sys import argv
from rijndael import rijndael
from stat import *
from struct import unpack

list = 0
verbose = 0
seperator = ":"
hiddendirs = {}

ROMFILESIZE = 40

def xorstr(str, enc):
	xorstr = ""
	for i in range(len(str)):
		xorstr += chr(ord(str[i])^ord(enc[i]))
	return xorstr

def long2chrl(lng, len):
	ret = ""
	for i in range(0,len*8,8):
		ret = ret + chr((lng>>i)&0xff)
	return ret

def chr2hex(chr):
	hex = ""
	for i in range(len(chr) - 1, -1, -1):
		hex += "%02x" % ord(chr[i])
	return hex

def chrl2long(chr):
	lng = 0
	for i in range(len(chr)):
		lng = (lng<<8) | ord(chr[i])
	return lng

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

def process_key(str):
	d = { 'u': 0, 'd': 1, 'l': 2, 'r': 3 }
	key = 0xFEBF5FAFD7EBF5FAFD7EBF5FAFD7EBF5
	for f in str:
		if f in d:
			key = ((key << 2) | d[f]) % (1<<126)
		else:
			print "Invalid input:", f
			print "Valid input is a combination of u, d, l, and r"
			sys.exit(3)
	return key

def process_joint(chunk, contents, position, length, rootstart, path):
	for i in range(0,length,ROMFILESIZE):
		(name, size, start) = unpack("<32sIi", chunk[i:i+ROMFILESIZE])
		name = make_valid(name)
		start += rootstart
		if start + (size & 0x7fffffff) > len(contents):
			print "Overflow"
			sys.exit(2)
		if size & 0x80000000:
			newpath = path + name + "/"
			if verbose and list:
				print "%08x\t%#8d\t%s" % (start, (size & 0x7fffffff) + 48, newpath)
			elif verbose or list:
				print newpath
			if not list:
				os.mkdir(name)
				os.chdir(name)
			process(contents, start, size & 0x7fffffff, rootstart, newpath)
			if not list:
				os.chdir("..")
		else:
			if verbose and list:
				print "%08x\t%#8d\t%s" % (start, size, path + name)
			elif verbose or list:
				print path + name
			if not list:
				fd = open(name, "wb")
				fd.write(contents[start:start+size])
				fd.close()

def process_hiddendir(chunk, contents, position, length, rootstart, path):
	process_joint(chunk, contents, position, length, rootstart, path)

def process(contents, position, length, rootstart, path):
	process_joint(contents[position:position+length], contents, position, length, rootstart, path)
	if position + length + (16*3) < len(contents):
		if path in hiddendirs:
			key = hiddendirs[path]
			start = position+length
			r = rijndael(key, block_size = 16)
			e = r.encrypt(contents[start:start+16])
			if e == contents[start+16:start+32]:
				d = r.decrypt(contents[start+32:start+48])
				name = "$key$" + chr2hex(key)
				newpath = path + name + "/"
				salt = chrl2long(d)
				(relative, salt1, salt2, realsize) = unpack("<4i", d)
				realsize &= 0x7fffffff
				size = (realsize + 15) & ~15
				start = rootstart + relative
				if start + size > len(contents):
					print "Overflow"
					sys.exit(2)
				dir = contents[start:start+size]
				tmp = ""
				temp = long2chrl(0xfedcba9876543210fedcba9876543210, 16)
				for i in range(0,size,16):
					dec = xorstr(r.decrypt(dir[i:i+16]), temp)
					temp = dir[i:i+16]
					tmp += dec
				chunk = tmp[0:realsize]
				if verbose and list:
					print "%08x\t%#8d\t%s" % (start, realsize, newpath)
				elif verbose or list:
					print newpath
				if not list:
					os.mkdir(name)
					os.chdir(name)
				process_hiddendir(chunk, contents, start, realsize, rootstart, newpath)
				if not list:
					os.chdir("..")

def usage(name):
	print "Usage: [-lvs:] [--verbose] [--list] [--seperator=c] [--hiddendir=/path/:keys] %s flashme.gba" % name

if __name__ == "__main__":
	hd = []
	try:
		opts, args = getopt.gnu_getopt(argv[1:], "lvs:", ["verbose", "list", "seperator=", "hiddendir="])
	except getopt.GetoptError:
		usage(argv[0])
		sys.exit(1)
	for o, a in opts:
		if o in ["-l", "--list"]:
			list = 1
		if o in ["-v", "--verbose"]:
			verbose = 1
		if o in ["-s", "--seperator"]:
			if len(a) != 1:
				usage(argv[0])
				sys.exit(1)
			seperator = a
		if o in ["--hiddendir"]:
			hd.append(a)
	if hd:
		for f in hd:
			l = f.split(seperator)
			if len(l) != 2:
				usage(argv[0])
				sys.exit(1)
			if not len(l[1]):
				usage(argv[0])
				sys.exit(1)
			value = process_key(l[1])
			hiddendirs[l[0]] = long2chrl(value, 16)
	if len(args) == 1:
		pos = 0
		size, contents = readfile(args[0])
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
				rootsize = unpack("<I", contents[pos+4:pos+8])[0]
				if verbose and list:
					print "%08x\t%#8d\t/" % (pos, (rootsize & 0x7fffffff) + 48 + 8)
				elif verbose or list:
					print "/"
				process(contents, pos+8, rootsize & 0x7fffffff, pos+8, "/")
	else:
		usage(argv[0])
