#! /usr/bin/env python

import binascii

class Binhack:
	
	def __init__(self, s):
		self.val = s
		
	def __repr__(self):
		return ' '.join([ bin(ord(c))[2:].zfill(8) for c in self.val ])
	
def KLEIN(state, sk):
	pass

def main():
	KEY = Binhack("\x00\x00\x00\x00\x00\x00\x00\x00")
	PLAINTEXT =	Binhack("\x12\x34\x56\x78\x90\xAB\xCD\xEF")
	print KEY
	print PLAINTEXT

if __name__ == '__main__':
	main()
	
	
	
# notes:
# fixed blocksize van 64 bit
# keylength van 64, 80 of 96 bit