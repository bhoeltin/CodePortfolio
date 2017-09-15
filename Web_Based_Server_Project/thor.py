#!/usr/bin/env python2.7

import multiprocessing
import os
import requests
import sys
import time
import urllib

# Globals

PROCESSES = 1
REQUESTS  = 1
VERBOSE   = False
URL       = None

# Functions

def usage(status=0):
    print '''Usage: {} [-p PROCESSES -r REQUESTS -v] URL
    -h              Display help message
    -v              Display verbose output

    -p  PROCESSES   Number of processes to utilize (1)
    -r  REQUESTS    Number of requests per process (1)
    '''.format(os.path.basename(sys.argv[0]))
    sys.exit(status)

def do_request(pid):
    tsum = 0
    count = 0
    for num in range(0, REQUESTS):
        start = time.time()
        r = requests.get(URL)
        end = time.time()
        timetaken = end - start
        tsum = tsum + timetaken
        count = count + 1
        print("Process: {}, Request: {}, Elapsed Time: {:.2f}".format(pid, num, timetaken))
    
    avtime = tsum / count
    print("Process: {}, AVERAGE   , Elapsed Time: {:.2f}".format(pid, avtime))
    return avtime

# Main execution

if __name__ == '__main__':
    # Parse command line arguments
    args = sys.argv[1:]
    while len(args) and args[0].startswith('-') and len(args[0]) > 1:
        arg = args.pop(0)
        if arg == '-h':
            usage(0)
        elif arg == '-v':
            VERBOSE = True
        elif arg == '-p':
            PROCESSES = int(args.pop(0))
        elif arg == '-r':
            REQUESTS = int(args.pop(0))
        else:
            usage(1)

    URL = args.pop(0)
    
    # Create pool of workers and perform requests
    if VERBOSE:
        link = urllib.urlopen(URL)
        myfile = link.read()
        print myfile   
 
    pool = multiprocessing.Pool(PROCESSES)
    timelist = pool.map(do_request, range(PROCESSES))
    avtotal = sum(timelist)/len(timelist)
    print("TOTAL AVERAGE ELAPSED TIME: {:.2f}".format(avtotal))
    #pass

# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
