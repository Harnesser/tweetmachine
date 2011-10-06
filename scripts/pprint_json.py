#! /usr/bin/env python
import sys
import pprint
import json

try:
    filename = sys.argv[1]
except IndexError:
    print "No JSON filename supplied!"
    sys.exit()
    
hJSON = open(filename, 'r')
data = json.loads(hJSON.read())

pprint.pprint(data)
