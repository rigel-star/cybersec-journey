# IPC using named pipes: the reader

import os

with open("/tmp/my_own_pipe", "r") as fifo:
	while True:
		content = fifo.readline(100)
		print(content)