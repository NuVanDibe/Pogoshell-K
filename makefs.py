"""
makefs.py

Builds a .gba, for use with Unofficial
Pogoshell v2.0b3-mod5.

Uses rijndael.py from the tlslite package

Usage: makefs.py [-rexmcva:] [--exclude=] pogo.gba root flashme.gba

Made by Kuwanger in Jan/Feb 2006
"""

import sys, os, getopt
from rijndael import rijndael
from sys import argv
from stat import *
from struct import pack
from random import SystemRandom

BINARY = 1
UNMOVABLE = 2
HIDDEN = 4
ENCRYPTED = 8
DIRECTORY = 16
LOWER = 32

ROMFILESIZE = 40
MAXHIDDENDIRS = 64
MAXHIDDENDIRSIZE = MAXHIDDENDIRS*ROMFILESIZE

verbose = 0
emptydir_remove = 0
moresecurepad = 0
align_mask = 32767
xrom = 0
exclude_ext = []
binary_ext = []

def hex2chr(string):
	ret = ""
	assert(len(string)%2 == 0)
	for i in range(len(string)/2):
		ret = ret + chr(int(string[i:i+2]))
	return ret

def intlist2chr(list):
	ret = ""
	for f in list:
		ret = ret + chr((f>>24)&0xff) + \
					 chr((f>>16)&0xff) + \
					 chr((f>>8)&0xff) + \
					 chr(f&0xff)
	return ret

def long2chrb(lng, len):
	ret = ""
	for i in range((len-1)*8,-8,-8):
		ret = ret + chr((lng>>i)&0xff)
	return ret

def long2chrl(lng, len):
	ret = ""
	for i in range(0,len*8,8):
		ret = ret + chr((lng>>i)&0xff)
	return ret

def xorstr(str, enc):
	xorstr = ""
	for i in range(len(str)):
		xorstr += chr(ord(str[i])^ord(enc[i]))
	return xorstr

def is_binary(name):
	extension = name.rfind(".")
	if name[extension:].lower() in binary_ext:
		return 1
	return 0

def is_invalid(name):
	if len(name) > 31:
		return 1
	for f in name:
		if not 32 <= ord(f) <= 127:
			return 1
	return 0

def readfile(name):
	try:
		fd = open(name, "rb")
		contents = fd.read()
		fd.close()
	except IOError:
		print "Error reading", name
		sys.exit(2)
	return os.stat(name)[ST_SIZE], contents

def IS_BINARY(foo):
	return (foo & BINARY) == BINARY

def IS_HIDDEN(foo):
	return (foo & HIDDEN) == HIDDEN

def IS_ENCRYPTED(foo):
	return (foo & ENCRYPTED) == ENCRYPTED

def IS_UNMOVABLE(foo):
	return (foo & UNMOVABLE) == UNMOVABLE

def IS_DIRECTORY(foo):
	return (foo & DIRECTORY) == DIRECTORY

def IS_LOWER(foo):
	return (foo & LOWER) == LOWER

def binary_first(left, right):
	if IS_UNMOVABLE(left.flags):
		return -1
	elif IS_UNMOVABLE(right.flags):
		return 1
	elif (IS_BINARY(left.flags) and IS_BINARY(right.flags)) or \
		 (not IS_BINARY(left.flags) and not IS_BINARY(right.flags)):
		if right.size < left.size:
			return -1
		else:
			return 1
	elif IS_BINARY(left.flags):
		return -1
	else:
		return 1

def romtrunc(contents, size):
	i = size - 1
	repeat = contents[i]
	if ord(repeat) == 0x00 or ord(repeat) == 0xff:
		i -=1
		while contents[i] == repeat:
			i -=1
	return i + 1

def pad(length):
	if length:
		if (moresecurepad):
			return long2chrl(SystemRandom(0).getrandbits(length*8), length)
		else:
			return "\xff" * length
	else:
		return ""

def bestfit(ls, length, l = 0):
	if len(ls) == 0:
		return [], [], length
	else:
		tlength = length
		for f in ls:
			tlength -= ((f.size + align_mask) & ~align_mask)
		if tlength < 0:
			if len(ls) == 1:
				return [], ls, length
			else:
				tlength1 = length - ((ls[0].size + align_mask)&~align_mask)
				lower, higher, tlength2 = bestfit(ls[1:], length, l + 1)
				if tlength1 >= 0 and tlength1 <= tlength2:
					return [ ls[0] ], ls[1:], tlength1
				else:
					return lower, [ ls[0] ] + higher , tlength2
		else:
			return ls, [], tlength

def pogocfg(path):
	path2 = "/.shell/pogo.cfg"
	try:
		fd = open(path + path2, "rb")
		fd.close()
	except IOError:
		try:
			path2 = "/.shell/pogo.cfg"
			fd = open(path + path2, "rb")
			fd.close()
		except IOError:
			return []
	cfg = readfile(path + path2)[1].splitlines()
	contents = {}
	list = []
	header = ""
	for f in cfg:
		dewhited = f.lstrip()
		#print f, ',', dewhited, ',', header, ',', list
		if dewhited:
			if dewhited[0] != '#':
				if dewhited[0] == '[' and dewhited[-1] == ']':
					new_header = dewhited[1:-1]
					if not new_header:
						print "You must specify a non-blank header in pogo.cfg"
						sys.exit(10)
					if header:
						contents[header] = list
					list = []
					header = new_header
				elif not header:
					print "You must specify a header before any information"
					sys.exit(9)
				else:
					list.append(f)
	if header:
		contents[header] = list
	return contents

class pogocompilation:
	'''pogofiles, pogodirs, pogo.gba, and the filesystem'''
	root = 0
	objects = []
	contents = ""
	def __init__(self, path, truncate, correct):
		cfg = pogocfg(path)
		if cfg:
			filetypes = cfg['filetypes']
			for f in filetypes:
				columns = f.split()
				if len(columns) < 3:
					print "Filetypes must have 3 or more columns."
					sys.exit(11)
				if columns[2] == "EXE":
					binary_ext.append("." + columns[0].lower())
			settings = cfg['settings']
			plugins_dir = ""
			for f in settings:
				dewhited = f.lstrip()
				if len(dewhited) > 7 and dewhited[0:8] == "PLUGINS=":
					plugins_dir = dewhited[8:]
					if plugins_dir and plugins_dir[-1] != '/':
						plugins_dir += '/'
					else:
						plugins_dir = '/'
					break
			if not plugins_dir:
				print "You must specify the plugins dir in pogo.cfg"
				sys.exit(6)
			try:
				wholefolder = os.listdir(path+"/"+plugins_dir)
			except :
				print "Plugins dir is unreadable"
				sys.exit(8)
		else:
			print "You must have /.shell/pogo.cfg or /.pogo.cfg in root"
			sys.exit(4)
		self.root = pogodir(path, "", plugins_dir, truncate, correct)
		self.root.name = "/"
		self.root.flags |= BINARY
		self.root.contents = pack("<II", 0xfab0babe, (self.root.size - 16*3) | 0x80000000)
		self.root.size += 8
		self.objects = self.root.allfilesandirs()
	def append(self, file):
		if self.objects:
			self.objects.append(file)
		else:
			self.objects = [ file ]
	def generateraw(self):
		self.root.generateraw()
	def organize(self):
		'''pack smaller files in gaps after binaries, otherwise leave the
		   order of files alone; sets each files .start'''
		neworder = []
		offset = 0
		if xrom:
			binary, nonbinary, lowerhalf, unmovable = [], [], [], []
			lower, higher = [], []
			unmovable_size, lowerhalf_size, nonbinary_size = 0, 0, 0
			for f in self.objects:
				if IS_UNMOVABLE(f.flags):
					unmovable.append(f)
					if IS_BINARY(f.flags):
						unmovable_size += (f.size + align_mask) & ~align_mask
					else:
						unmovable_size += (f.size + 3) & ~3
				elif IS_LOWER(f.flags) and IS_BINARY(f.flags):
					lowerhalf.append(f)
					lowerhalf_size += (f.size + align_mask) & ~align_mask
				elif IS_BINARY(f.flags):
					binary.append(f)
				else:
					nonbinary.append(f)
					nonbinary_size += (f.size + 3) & ~3
			freelower = 32*1024*1024 - nonbinary_size - lowerhalf_size - unmovable_size
			if freelower < 0:
				print "Non-binary data must be below 32MB"
				sys.exit(1)
			lower, higher, rest = bestfit(binary, freelower)
			lower.extend(lowerhalf)
			lower.sort(binary_first)
			for f in unmovable:
				if IS_BINARY(f.flags):
					offset = (offset+align_mask) & ~align_mask
				else:
					offset = (offset+3) & ~3
				f.start = offset
				offset += f.size
				neworder.append(f)
			for f in lower:
				offset = (offset+3) & ~3
				if IS_BINARY(f.flags):
					gap = ((offset + align_mask) & ~align_mask) - offset
					l = len(nonbinary)
					i = 0
					while gap != 0 and i < l:
						current = nonbinary[i]
						if current.size <= gap:
							current.start = offset
							offset += current.size
							offset = (offset+3) & ~3
							gap = ((offset + align_mask) & ~align_mask) - offset
							neworder.append(current)
							del nonbinary[i]
							l -= 1
						else:
							i += 1
					offset += gap
				f.start = offset
				offset += f.size
				neworder.append(f)
			for f in nonbinary:
				offset = (offset+3) & ~3
				f.start = offset
				offset += f.size
				neworder.append(f)
			if higher:
				offset = 32*1024*1024
				for f in higher:
					offset = (offset+align_mask) & ~align_mask
					f.start = offset
					offset += f.size
					neworder.append(f)
		else:
			while self.objects:
				f = self.objects.pop(0)
				if IS_BINARY(f.flags):
					offset = (offset+3) & ~3
					gap = ((offset + align_mask) & ~align_mask) - offset
					l = len(self.objects)
					i = 0
					while gap != 0 and i < l:
						current = self.objects[i]
						if not IS_BINARY(current.flags) and current.size <= gap:
							current.start = offset
							offset += current.size
							offset = (offset+3) & ~3
							gap = ((offset + align_mask) & ~align_mask) - offset
							neworder.append(current)
							del self.objects[i]
							l -= 1
						else:
							i += 1
					offset += gap
				offset = (offset+3) & ~3
				f.start = offset
				offset += f.size
				neworder.append(f)
		self.objects = neworder
	def setstarts(self):
		'''resort, packing in free space after binaries, and
		   set both the absolute start from the start of the file
		   as well as relative from the root's first entry'''
		self.organize()
		self.root.setrelative(self.root.start + 8)
	def sort(self):
		self.objects.sort(binary_first)
	def write(self, name):
		offset = 0
		try:
			fd = open(name, "wb")
			for f in self.objects:
				if offset < f.start:
					fd.write(pad(f.start-offset))
					offset = f.start
				if verbose:
					print f.name, f.start, f.size
				fd.write(f.contents)
				offset += f.size
			fd.write(pad(((offset+3)&~3)-offset))
			fd.close()
		except IOError:
			print "Error writing", name
			sys.exit(3)
		offset = (offset+3)&~3
		print "%dMBit(%dMB) cart needed" % (offset/131072, offset/1048576)

class pogoobject:
	'''base class for all files/dirs/components of a pogoshell image'''
	def __init__(self, name):
		self.name = ""
		self.size = 0
		self.start = 0
		self.relative = 0
		self.flags = 0
		self.contents = ""
	def fillname(self, name, correct):
		'''for non-hidden files, limit to 31 character names, trying to keep
		   intact the extension, if possible
		   for hidden files, just include whole since it's never processed'''
		if IS_HIDDEN(self.flags):
			return name
		else:
			slash = name.rfind("/")
			slash2 = name.rfind("\\")
			if slash2 > slash:
				slash = slash2
			name = name[slash+1:]
			new_name = name
			extension = name.rfind(".")
			if extension != -1:
				if len(name)-extension > 31:
					new_name = name[extension:extension+31]
			else:
				extension = len(name)
			if len(name) > 31:
				new_name = name[:31-len(name)+extension] + name[extension:]
			tmp_name = ""
			for f in new_name:
				if not 32 <= ord(f) <= 127 or (correct and f == "_"):
					tmp_name += autocorrectchar
				else:
					tmp_name += f
			new_name = tmp_name
			if correct:
				if name.rfind('_') != -1 or is_invalid(name):
					invalid = 1
					print "Old name: %s" % name
					while invalid:
						name = raw_input("New name[%s]: " % new_name)
						if name == "":
							name = new_name
						invalid = is_invalid(name)
						if invalid:
							print "Invalid name.  Name must best 31 characters or shorter and only use ascii characters 32 to 127."
					new_name = name
			return new_name

class pogodir(pogoobject):
	'''the directory tree'''
	def __init__(self, path, relative_to_root, plugins_dir, truncate, correct, inhidden = 0):
		pogoobject.__init__(self, path)
		self.subdirs = []
		self.hiddendir = 0
		self.key = 0
		self.files = []
		self.flags |= DIRECTORY | LOWER
		file = 0
		dir = 0
		count = 0
		stats = []
		wholefolder = os.listdir(path)
		empty_hiddendir = 0
		for f in wholefolder:
			stats.append([ f, os.stat(path + "/" + f)[ST_MODE] ])
		for f in stats:
			if S_ISDIR(f[1]):
				if len(f[0]) == (5+32) and f[0][:5] == "$key$":
					if not inhidden and not self.hiddendir:
						hiddenkey = f[0][5:]
						try:
							value = long(hiddenkey, 16)
						except Exception:
							print "Invalid key", f[0]
							sys.exit(5)
						self.hiddendir = pogodir(path + "/" + f[0], relative_to_root+f[0] + "/", plugins_dir, truncate, correct, 1)
						if emptydir_remove and not self.hiddendir.subdirs and not self.hiddendir.files:
							empty_hiddendir = 1
						else:
							self.hiddendir.size -= (16*3)
							if self.hiddendir.size > MAXHIDDENDIRSIZE:
								print "Hidden dir can only be %d entries long.  You must remove at least %d.  Aborting." % ( MAXHIDDENDIRS, self.hiddendir.size / ROMFILESIZE - MAXHIDDENDIRS )
								sys.exit(1)
							self.hiddendir.realsize = self.hiddendir.size
							self.hiddendir.flags |= HIDDEN | ENCRYPTED
							self.hiddendir.name = self.fillname(f[0], 0)
							self.hiddendir.size = (self.hiddendir.size + 15) & ~15
							self.hiddendir.key = long2chrl(value & ((1 << 128) - 1 - (3<<126)), 16)
							self.key = self.hiddendir.key
				else:
					dir = pogodir(path + "/" + f[0], relative_to_root + f[0] + "/", plugins_dir, truncate, correct, inhidden)
					if not emptydir_remove or dir.subdirs or dir.files or dir.hiddendir:
						count += 1
						dir.name = dir.fillname(f[0], correct)
						self.subdirs.append(dir)
			elif S_ISREG(f[1]):
				if relative_to_root == plugins_dir or f[0][f[0].rfind("."):] not in exclude_ext:
					count += 1
					file = pogofile(path + "/" + f[0], 0, truncate, correct)
					if relative_to_root == plugins_dir:
						#print "Plugin:", f[0]
						file.flags |= LOWER
					self.files.append(file)
		if empty_hiddendir:
			self.hiddendir = 0
		self.size = count*ROMFILESIZE + (16*3)
	def generateraw(self):
		'''generate the correctly formatted subdirectory, telling
		   all subdirectories to do the same'''
		tempraw = ""
		for f in self.subdirs:
			tempraw += pack("<32sIi", f.name, (f.size - 16*3) | 0x80000000, f.relative)
			f.generateraw()
		for f in self.files:
			tempraw += pack("<32sIi", f.name, f.size, f.relative)
		if IS_ENCRYPTED(self.flags):
			size = self.realsize
			if size < self.size:
				junk = long2chrl(SystemRandom(0).getrandbits((self.size - size)*8), self.size - size)
				tempraw += pack("<%ds" % (self.size - size), junk)
				size = self.size
			r = rijndael(self.key, block_size = 16)
			tmp = ""
			enc = long2chrl(0xfedcba9876543210fedcba9876543210, 16)
			for i in range(0,size,16):
				temp = xorstr(tempraw[i:i+16], enc)
				enc = r.encrypt(temp)
				tmp += enc
			self.contents += tmp
		else:
			salt2 = 0
			self.contents += tempraw
			salt = long2chrl(SystemRandom(0).getrandbits(128), 16)
			if self.hiddendir:
				self.hiddendir.generateraw()
				key = self.key
				salt2 = ((self.hiddendir.realsize | 0x80000000) << 96) | \
						(SystemRandom(0).getrandbits(64) << 32) | \
						(self.hiddendir.relative & 0xffffffff)
			else:
				salt2 = SystemRandom(0).getrandbits(128)
				key = long2chrl(SystemRandom(0).getrandbits(128) | (3<<126), 16)
			salt2 = long2chrl(salt2, 16)
			r = rijndael(key, block_size = 16)
			encsalt = r.encrypt(salt)
			encsalt2 = r.encrypt(salt2)
			self.contents += pack("<16s16s16s", salt, encsalt, encsalt2)
	def setrelative(self, rootstart):
		self.relative = self.start - rootstart
		if self.hiddendir:
			self.hiddendir.setrelative(rootstart)
		for f in self.subdirs:
			f.setrelative(rootstart)
		for f in self.files:
			f.relative = f.start - rootstart
	def allfiles(self):
		collection = self.files[:]
		if self.hiddendir:
			collection.extend(self.hiddendir.allfiles())
		for f in self.subdirs:
			collection.extend(f.allfiles())
		return collection
	def alldirs(self):
		collection = [ self ]
		if self.hiddendir:
			collection.extend(self.hiddendir.alldirs())
		for f in self.subdirs:
			collection.extend(f.alldirs())
		return collection
	def allfilesandirs(self):
		collection = self.alldirs()
		collection.extend(self.allfiles())
		return collection

class pogofile(pogoobject):
	'''real files'''
	def __init__(self, name, flags, truncate, correct):
		pogoobject.__init__(self, name)
		self.flags |= flags
		if is_binary(name):
			self.flags |= BINARY
		self.name = self.fillname(name, correct)
		self.size, self.contents = readfile(name)
		if (truncate and IS_BINARY(self.flags)):
			self.size = romtrunc(self.contents, self.size)
			self.contents = self.contents[0:self.size]

def usage(name):
	print "Usage: %s [-rxmvc[a<c>]] [--exclude=...] pogo.gba root root.gba" % name

if __name__ == "__main__":
	i = 1
	truncate = 0
	correct = 0
	autocorrectchar = ""
	try:
		opts, args = getopt.gnu_getopt(argv[1:], "rsxmcva:", ["romtrunc", "correct", "subdirempty", "moresecurepad", "verbose", "xrom", "autocorrectchar=", "exclude="])
	except getopt.GetoptError:
		usage(argv[0])
		sys.exit(1)
	for o, a in opts:
		if o in ("-r", "--romtrunc"):
			truncate = 1
		if o in ("-c", "--correct"):
			correct = 1
		if o in ("-v", "--verbose"):
			verbose = 1
		if o in ("-s", "--subdirempty"):
			emptydir_remove = 1
		if o in ("-a", "--autocorrectchar"):
			autocorrectchar = a
			if len(autocorrectchar) != 1:
				usage(argv[0])
				sys.exit(1)
		if o in ("-m", "--moresecurepad"):
			moresecurepad = 1
		if o in ("-x", "--xrom"):
			align_mask = 128*1024-1
			xrom = 1
		if o in ("--exclude"):
			ls = a.split(",")
			for f in ls:
				if len(f) > 0:
					if f[0] != ".":
						exclude_ext.append("." + f)
					else:
						exclude_ext.append(f)
				else:
					print "You must specify a non-zero length extension."
	if autocorrectchar and not correct:
		usage(argv[0])
		sys.exit(1)
	if correct and not autocorrectchar:
		autocorrectchar="_"
	if len(args) == 3:
		mainfile = pogofile(args[0], BINARY | HIDDEN | LOWER | UNMOVABLE, truncate, correct)
		root = pogocompilation(args[1], truncate, correct)
		root.append(mainfile)
		root.sort()
		root.setstarts()
		root.generateraw()
		root.write(args[2])
	else:
		usage(argv[0])
