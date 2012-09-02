#! /usr/bin/env python
import sys

if __name__ == '__main__':
	filename = sys.argv[1]
	in_font = False

	hH = open(filename, 'r')
	char_id = 0
	for line in hH:
		line = line.strip()
		if 'FONT_5X4 []' in line:
			in_font = True
		elif '};' in line:
			in_font = False
		elif in_font:
			data, comment = line.split('//')
			bits = data.split(',')
			print 'const char CHAR%0d [] PROGMEM = { %s }; // %s' % (
				char_id, ','.join(bits), comment )
			char_id += 1
	hH.close()

	print "const char* FONT_5X4 [] PROGMEM = {"
	for cid in xrange(0, char_id):
		print "   CHAR%d" % (cid)
	print "};"
