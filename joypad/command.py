#!/usr/bin/python
# *-* encoding: utf-8 *-*

from pypad import *
from terminalcontrol import *

#robot = ('10.0.0.1', 2000)
robot = ('127.0.0.1', 2000)
joypad = '/dev/input/js0'


# Open the connection
try:
    terminal = TerminalControl(robot, 30)
except:
    print('Unable to connect to the robot')
    exit

# Create the Pad
try:
    pad = PyPad(joypad)
except:
    print('Unable to open the Joypad')
    exit

pad.onButtonPressed(14, lambda: terminal.send('Hello!'))
pad.onButtonReleased(14, lambda: terminal.send('GoodBye!'))
pad.onAxis(0, lambda v: terminal.send('amp1', v))
pad.onAxis(1, lambda v: terminal.send('amp2', v))
pad.onAxis(2, lambda v: terminal.send('amp3', v))
pad.onAxis(3, lambda v: terminal.send('amp4', v))

try:
    while True:
        event = pad.getEvent()
except:
    print('Error or interruption, aborting...')
    terminal.running = False
