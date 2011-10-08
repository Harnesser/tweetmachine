#! /usr/bin/env python
"""
Convert a font bitmap thing.
"""

orig_data = {}
hORIG = open('font_5x4.h');

import pprint

def reflow(string):
    bin = []
    string = string.replace('0b','')
    for char in string:
        if char == '0' or char == '1':
            bin.append(char)
            
    sideways = []
    for i in range(0,len(bin), 8 ):
        top = bin[i:i+4]
        bot = bin[i+4:i+8]
        top.reverse()
        bot.reverse()
        top.extend(bot)
        sideways.append(top)
        
    upright = zip(*sideways)
    
    del upright[-1]
    del upright[-1]
    del upright[-1]
        
    defn = []
    for line in upright:
        defn_str = ''.join(line)
        defn.append('0b%s000' % ( ''.join(defn_str) ) ) 

    return " { %s }, " % (', '.join(defn)) 
        
in_data = False
for line in hORIG:
    line = line.replace('\n','')
    line = line.lstrip()
    line = line.rstrip()

    if in_data:
        try:
            data, char = line.split('//')
        except ValueError:
            continue

        char = char.lstrip()
        flipped_data = reflow(data)
        orig_data[char] = flipped_data
        
        print flipped_data, "// ", char
        
    if line.startswith('char FONT_5X4'):
        in_data = True

   
hORIG.close()

