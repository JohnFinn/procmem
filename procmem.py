#!/usr/bin/env python3

import os

class ProcMem:
	"""
	requires root priveleges !!

	API:
	
	pid = 2000
	with ProcMem(pid) as pm:
		offset = 0x7ffd025ce62c
		length = 100
		print(pm[offset, length])
		pm[offset] = b'Some other data'
		print(pm[offset, length])

	pm = ProcMem(pid)
	for address in pm.findAll(b'String to be found'):
		pm[address] = b'Replacer string'

	"""


	def __init__(self, pid: int):
		self.__pid = pid
		self.__file = open('/proc/{}/mem'.format(pid), 'br+')

	def __getitem__(self, addr_size):
		if isinstance(addr_size, int):
			addr = addr_size
			size = 1
		else:
			addr, size = addr_size
		self.__file.seek(addr)
		return self.__file.read(size)

	def __setitem__(self, addr, data):
		self.__file.seek(addr)
		self.__file.write(data)
		self.__file.flush()

	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.close()

	def close(self):
		self.__file.close()

	def intervals(self):
		with open('/proc/{}/maps'.format(self.__pid)) as file:
			for line in file:
				interval, perms, *rest = line.split()
				i1, i2 = interval.split('-')
				yield (int(i1, 16), int(i2, 16)), perms

	def findAll(self, data):
		for (start, end), perms in self.intervals():
			if 'r' not in perms:
				continue
			try:
				self.__file.seek(start)
			except ValueError:
				continue
			try:
				b = self.__file.read(end - start)
			except OSError:
				continue

			for index in self.substring_indexes(data, b):
				yield start + index

	@staticmethod
	def substring_indexes(substring, string):
		""" 
		Generate indices of where substring begins in string

		>>> list(ProcMem.substring_indexes('me', "The cat says meow, meow"))
		[13, 19]
		"""
		last_found = -1  # Begin at -1 so the next position to search from is 0
		while True:
			# Find next index of substring, by starting after its last known position
			last_found = string.find(substring, last_found + 1)
			if last_found == -1:  
				break  # All occurrences have been found
			yield last_found


def scan_all_processes(data):
	result = {}
	for pid in os.listdir('/proc/'):
		if not pid.isdigit():
			continue
		print(pid, end='\r')
		pid = int(pid)
		pm = ProcMem(pid)
		addreses = tuple(pm.findAll(data))
		pm.close()
		if addreses:
			result[pid] = addreses
	return result

if __name__ == '__main__':
	import sys
	scanned = scan_all_processes(sys.argv[1].encode())
	replacer = sys.argv[2].encode()
	for pid in scanned:
		for addr in scanned[pid]:
			with ProcMem(pid) as pm:
				pm[addr] = replacer
