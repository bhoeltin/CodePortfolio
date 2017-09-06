#!/usr/bin/env python2.6
import string
import os
import sys
import re

# python script that automates the process of producing the output

os.system("./script | tee textfile.txt") # uses a system call to run the script and send the output to a text file
os.system("./callGoogleImg.py") # uses a system call to find the images for the output
os.system("cat textfile.txt | grep ! | ./createHTML.py") # uses a system call to display the output, find the food items, and then create the HTML output page




