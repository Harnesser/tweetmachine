#! /usr/bin/env python
""" Send a tweet to the arduino over the serial link. """

import serial

tweet = '''u:harnesser
o:RT @trevorpower The electronics version of "step into" #debug #breadboard #
g:milklabs
'''
link = None
serial_ports = ['/dev/ttyUSB0', '/dev/ttyACM0']

for port in serial_ports:
    if link is None:
        try:
            link = serial.Serial(port, baudrate=9600, timeout=2)
        except serial.serialutil.SerialException:
            pass

def send_tweet():
    #while link.readline().find('Tweetme') < 0:
    #    pass        
    link.write(tweet)

if __name__ == '__main__':
	send_tweet()        
