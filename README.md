# procmem
Process memory reader &amp; modifier for linux


example:
```python	
pid = 2000 # process if we want to read / change memory
with ProcMem(pid) as pm: # closes /proc/pid/mem in __exit__ method
	offset = 0x7ffd025ce62c
	length = 100
	print(pm[offset, length])
	pm[offset] = b'Some other data'
	print(pm[offset, length])

pm = ProcMem(pid)
for address in pm.findAll(b'String to be found'):
	pm[address] = b'Replacer string'
pm.close()
```
