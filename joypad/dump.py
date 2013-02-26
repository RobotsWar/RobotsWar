#!/usr/bin/python

from pypad import *

try:
    pad = PyPad('/dev/input/js0')
except:
    print('Unable to open the Joypad')
    exit

while True:
    event = pad.getEvent()
    print('Event %s index=%d: %f' % (event.eventType, event.index, event.value))
