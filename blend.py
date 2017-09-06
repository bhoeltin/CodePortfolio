#!/usr/bin/env python2.7

import atexit
import os
import re
import shutil
import sys
import tempfile

import requests


# Global variables

REVERSE     = False
DELAY       = 20
STEPSIZE    = 5

# Functions

def usage(status=0):
    print '''Usage: {} [ -r -d DELAY -s STEPSIZE ] netid1 netid2 target
    -r          Blend forward and backward
    -d DELAY    GIF delay between frames (default: {})
    -s STEPSIZE Blending percentage increment (default: {})'''.format(
        os.path.basename(sys.argv[0]), DELAY, STEPSIZE
    )
    sys.exit(status)

def cleanup():
    print "Cleaning up workspace: {}".format(workspace)
    shutil.rmtree(workspace)

def search_portrait(netid):
    url = 'http://engineering.nd.edu/profiles/{}'.format(netid)
    try: 
        response = requests.get(url)
        regex = '(http:.*/{}/.*\.*g)"'.format(netid)
        print "Searching portrait for {}...".format(netid),
        iurl = re.findall(regex, response.text)
        print "{}".format(iurl[0])
        return iurl[0]
    except:
        print "Error!"
        sys.exit(1)

def download_file(url, path):
    r = requests.get(url)
    try:
        with open(path, 'wb') as outfile:
            outfile.write(r.content)
        print "Downloading {}".format(url)
    except:
        print "Unable to download file"
        sys.exit(1)

def run_command(command):
    try: 
        os.system(command)
    except:
        print "Error"
        sys.exit(1)


# Parse command line options

args = sys.argv[1:]

while len(args) and args[0].startswith('-') and len(args[0]) > 1:
    arg = args.pop(0)
    if arg == '-r':
        REVERSE = True
    elif arg == '-d':
        DELAY = int(args.pop(0))
    elif arg == "-s":
        STEPSIZE = int(args.pop(0))
    elif arg == "-h":
        usage(0)
    else:
        usage(1)

if len(args) != 3:
    usage(1)

netid1 = args[0]
netid2 = args[1]
target = args[2]

# Main execution

workspace = tempfile.mkdtemp(prefix='blend')

print "Using workspace: {}".format(workspace)
atexit.register(cleanup)
              

# TODO: Extract portrait URLs

url1 = search_portrait(netid1)
url2 = search_portrait(netid2)

# TODO: Download portraits

path1 = os.path.join(workspace, '{}.jpg'.format(netid1)) 
path2 = os.path.join(workspace, '{}.jpg'.format(netid2))

download_file(url1, path1)
download_file(url2, path2)

# TODO: Generate blended composite images

GIFLIST = []

GIFLIST.append('convert -loop 0 -delay {}'.format(DELAY))

RANGESTEP = 100/STEPSIZE + 1

for i in range(RANGESTEP):
    NUM = "{:03d}".format(STEPSIZE*i)
    output = '{}-{}-{}.gif'.format(NUM,netid1,netid2)
    savepath = os.path.join(workspace, output)
    GIFLIST.append(savepath)
    print "Generating {}...".format(savepath),
    run_command('composite -blend {} {} {} {}'.format(NUM,path1,path2,savepath))
    print "Success!"

if REVERSE == True:
    for i in range(RANGESTEP):
        NUM= "{:03d}".format(100-STEPSIZE*i)
        output = '{}-{}-{}.gif'.format(NUM,netid1,netid2)
        savepath = os.path.join(workspace, output)
        GIFLIST.append(savepath)
        run_command('composite -blend {} {} {} {}'.format(NUM,path1,path2,savepath))
    GIFLIST.append(target)
    print "Generating {}...".format(target),
    run_command(" ".join(GIFLIST))
    print "Success!"
else:
    GIFLIST.append(target)
    print "Generating {}...".format(target),
    run_command(" ".join(GIFLIST))
    print "Success!"
