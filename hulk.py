#!/usr/bin/env python2.7

import functools
import hashlib
import itertools
import multiprocessing
import os
import string
import sys

# Constants

ALPHABET    = string.ascii_lowercase + string.digits
ARGUMENTS   = sys.argv[1:]
CORES       = 1
HASHES      = 'hashes.txt'
LENGTH      = 1
PREFIX      = ''

# Functions

def usage(exit_code=0):
    print '''Usage: {} [-a alphabet -c CORES -l LENGTH -p PATH -s HASHES]
    -a ALPHABET Alphabet to use in permutations
    -c CORES    CPU Cores to use
    -l LENGTH   Length of permutations
    -p PREFIX   Prefix for all permutations
    -s HASHES   Path of hashes file'''.format(os.path.basename(sys.argv[0]))
    sys.exit(exit_code)

def md5sum(s):
    return hashlib.md5(s).hexdigest()

def permutations(length, alphabet=ALPHABET):
    
    if length <= 1:
        for letter in alphabet:
            yield letter
    else:
        for letter in alphabet:
            for perm in permutations(length-1, alphabet):
                yield letter + perm

def smash(hashes, length, alphabet=ALPHABET, prefix=''):
    # TODO: Implement
    
    perms = permutations(length, alphabet)
    perm = (prefix + p for p in perms)
        
    return [p for p in perm if md5sum(p) in hashes]
    
    

# Main Execution

if __name__ == '__main__':
    while len(ARGUMENTS) and ARGUMENTS[0].startswith('-') and len(ARGUMENTS[0]) > 1:
        arg = ARGUMENTS.pop(0)
        if arg == '-a':
            ALPHABET = ARGUMENTS.pop(0)
        elif arg == '-c':
            CORES = int(ARGUMENTS.pop(0))
        elif arg == "-l":
            LENGTH = int(ARGUMENTS.pop(0))
        elif arg == '-p':
            PREFIX = ARGUMENTS.pop(0)
        elif arg == '-s':
            HASHES = ARGUMENTS.pop(0) 
        elif arg == "-h":
            usage(0)
        else:
            usage(1)
            
    with open(HASHES, 'r') as f:
        hashes = set([h.strip() for h in f])

# TODO: Execute smash function to get passwords 
    pool = multiprocessing.Pool(CORES)

    if CORES > 1 & LENGTH > 1:
        if LENGTH % 2 == 0:
            length = LENGTH/2
        else:
            length = length/2 +1
        subsmash= functools.partial(smash,hashes, length, ALPHABET)
        passwords = itertools.chain.from_iterable(pool.imap(subsmash, (PREFIX + p for p in permutations(LENGTH-length, ALPHABET))))
        
    else:
        passwords = smash(hashes, LENGTH, ALPHABET, PREFIX)
        
# TODO: Print passwords
    for password in passwords:
        print password
    
# vim: set sts=4 sw=4 ts=8 expandtab ft=python:
