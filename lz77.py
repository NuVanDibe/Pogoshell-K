
import os,profile
from stat import *
from sys import argv
from struct import pack

def readfile(name):
	fd = open(name, "rb")
	contents = fd.read()
	fd.close()
	return os.stat(name)[ST_SIZE], contents

def writefile(name, contents):
	fd = open(name, "wb")
	fd.write(contents)
	fd.close()

class lz77_gba:
	MAXWINDOW = 4096
	MATCHLEN = 19
	def search(self, window_dict, string, range_to_use):
		'''find the longest matching substring'''
		for f in range_to_use:
			if string[:f] in window_dict:
				return window_dict[string[:f]]
		return (-1, 0)
	def remove_old(self, window_dict, string, start, range_to_use):
		'''remove hashes for all 3+ long substrings
		   Note: This is safe because if a substring exists further
				 in the window, it's already overwrote the hash
				 position.'''
		for f in range_to_use:
			if string[:f] in window_dict:
				if window_dict[string[:f]][0] == start:
					del window_dict[string[:f]]
		return window_dict
	def add_new(self, window_dict, string, start, range_to_use):
		'''add hashes for all 3+ long substrings'''
		for f in range_to_use:
			window_dict[string[:f]] = (start, f)
		return window_dict
	def compress(self, z):
		'''the actual lz77 gba compressor

		   returns compressed string'''
		window_dict = {}
		flags = 0
		index = 0
		lenz = len(z)
		revstdrange = range(self.MATCHLEN,2,-1)
		stdrange = range(3,self.MATCHLEN)
		#tag header as lz77 compressed along with size
		compressed = pack("<I", (lenz<<8)+0x10)
		flags = 0
		lastchunk = ""
		if index < lenz:
			lastchunk += z[index]
			index += 1
			ormask = 128
			while index < lenz:
				ormask >>= 1
				if ormask == 0:
					ormask = 128
					#append to the final compressed string once we
					#have the flags done for this chunk
					compressed += chr(flags)
					compressed += lastchunk
					flags = 0
					lastchunk = ""
				end = index + self.MATCHLEN
				range_to_use = revstdrange
				if end > lenz:
					end = lenz
					range_to_use = range(end-index,2,-1)
				rindex, rlength = self.search(window_dict, z[index:end], \
											range_to_use)
				if rlength > 2:
					flags |= ormask
					displacement = index - rindex - 1
					#pack compression tag
					cvalue = ((displacement & 0xff) << 8) | \
								(displacement >> 8) | \
								((rlength - 3) << 4)
					lastchunk += pack("<h", cvalue) 
				else:
					lastchunk += z[index]
					#rest rlength, since the match string was too short
					#for compression, so we did a byte (ie, one) copy
					rlength = 1
				for f in range(rlength):
					start = index - self.MAXWINDOW
					#only remove outside-of-window stuff
					if start >= 0:
						range_to_use = stdrange
						end = start + self.MATCHLEN
						if end > lenz:
							end = lenz
							range_to_use = range(3,end-start)
						window_dict = self.remove_old(window_dict, \
										z[start:end], start, range_to_use)
					index += 1
					start = index - 1
					#only add if the string start is in the window
					if start >= 0:
						range_to_use = stdrange
						end = start + self.MATCHLEN
						if end > lenz:
							end = lenz
							range_to_use = range(3,end-start)
						window_dict = self.add_new(window_dict, z[start:end], \
											start, range_to_use)
		#add partial flag and chunk
		compressed += chr(flags)
		compressed += lastchunk
		data = len(compressed)
		#and word align the size
		newlength = (data + 3) & ~3
		while data < newlength:
			compressed += "\0"
			data += 1
		return compressed

if __name__ == "__main__":
	if len(argv) == 3:
		size, contents = readfile(argv[1])
		writefile(argv[2], lz77_gba().compress(contents))
