#!/usr/bin/env python2.7

import atexit
import os
import re
import shutil
import sys
import tempfile

import requests

FIELD = 'title'
LIMIT = 10
SUBREDDIT = 'linux'
SEARCH = ''

# Functions

def usage(status=0):
    print '''Usage: reddit.py [ -f FIELD -s SUBREDDIT ] regex
    -f FIELD        Which field to search (default: title)
    -n LIMIT        Limit number of articles to report (default: 10)
    -s SUBREDDIT    Which subreddit to search (default: linux)'''.format(
        os.path.basename(sys.argv[0]), FIELD, LIMIT, SUBREDDIT
    )
    sys.exit(status)

args = sys.argv[1:]

while len(args) and args[0].startswith('-') and len(args[0]) > 1:
    arg = args.pop(0)
    if arg == '-f':
        FIELD = args.pop(0)
    elif arg == '-n':
        LIMIT = int(args.pop(0))
    elif arg == "-s":
        SUBREDDIT = args.pop(0)
    elif arg == "-h":
        usage(0)
    else:
        usage(1)

if len(args) == 1:
    SEARCH = args[0]


headers  = {'user-agent': 'reddit-{}'.format(os.environ['USER'])}
response = requests.get('https://www.reddit.com/r/{}/.json'.format(SUBREDDIT), headers=headers)

info = response.json()


count = 1

for articles in info['data']['children']:
    if FIELD not in articles['data'].keys():
        print "Invalid field: {}".format(FIELD)
        sys.exit(1)
    if re.search(SEARCH, articles['data']['{}'.format(FIELD)]):
        TITLE = articles['data']['title']
        AUTHOR = articles['data']['author']
        LINK = 'https://www.reddit.com'+articles['data']['permalink']
        pars = {'format': 'json', 'url': LINK}
        SHORT = requests.get('http://is.gd/create.php', params=pars).json()

        print '''{}.   Title: {}    
     Author: {}                                                       
     Link: {}
     Short: {}'''.format(count,TITLE,AUTHOR,LINK,SHORT['shorturl'])
        count += 1
    if count > (LIMIT):
        sys.exit(1)
