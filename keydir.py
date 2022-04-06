
import sys, os, getopt
from sys import argv

def usage(name):
	print "Usage: %s [-m]" % name

if __name__ == "__main__":
	make = 0
	d = { 'u': 0, 'd': 1, 'l': 2, 'r': 3 }
	key = 0xFEBF5FAFD7EBF5FAFD7EBF5FAFD7EBF5
	input = raw_input("Key: ")
	for f in input:
		if d.has_key(f):
			key = ((key << 2) | d[f]) % (1<<126)
		else:
			print "Invalid input:", f
			print "Valid input is a combination of u, d, l, and r"
			sys.exit(1)
	str = "$key$%032x" % key
	try:
		opts, args = getopt.gnu_getopt(argv[1:], "m", ["make"])
	except getopt.GetoptError:
		usage(argv[0])
		sys.exit(1)
	for o, a in opts:
		if o in ("-m", "--make"):
			make = 1
	if make:
		os.mkdir(str)
	else:
		print str
