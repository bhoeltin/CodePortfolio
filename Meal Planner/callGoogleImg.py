#!/usr/bin/env python2.7

import string
import sys
import csv
import os
import re

filename = 'textfile.txt'
out = 'url.txt'
urls = []


tail1 = "A"
tail2 = "B"

#ensures empty file at the beginning
os.system("echo '' > url.txt")
counter = 1
with open(filename, 'r') as f:
	count = 0
	# looks through each of the lines in the file
	for line in f.readlines():
		if count > 3:
			# split line so that the calories are not in the name 
			line = line.split('!')
			line = line[0]
			line = line.encode('ascii', 'backslashreplace')
			# command to put all the image urls into a file to be parsed later
			string = "echo {} | ./GoogleImg.py | grep http | head -1 >> url.txt".format(line)
			string = os.system(string)
			# this accounts for the API being unable to find an image and generate a URI
			# this puts a blank line instead
			if counter%2 == 0: 
				tail2 = os.popen("cat url.txt | tail -1")
				tail2 = tail2.readline()
				if tail1 == tail2:
					os.system("echo '' >> url.txt")
			else:
				tail1 = os.popen("cat url.txt | tail -1")
				tail1 = tail1.readline()
				if tail1 == tail2:
					os.system("echo '' >> url.txt")
		counter = counter + 1
		count = count + 1

