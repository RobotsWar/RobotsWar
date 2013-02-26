#!/usr/bin/python

"""
    A pad event, fields are :

    index: the index of the button or axis
    eventType: the type of the event (button or axis)
    value: the value, True or False for buttons and char fo axis
"""
class PyPadEvent:
    def __init__(self, msg):
        self.index = msg[7]
        self.eventType = None
        self.value = None
        if msg[6] == 1: # Button
            self.eventType = 'button'
            self.value = (msg[4]==1)
        if msg[6] == 2: # Axis
            self.eventType = 'axis'
            if msg[5] > 127:
                msg[5] = msg[5]-255
            self.value = (msg[5]/128.0)

"""
    A pad

    Usage:
        pad = PyPad('/dev/input/js0')
        
        while True:
            event = pad.getEvent()
            ...
"""
class PyPad:
    def __init__(self, js):
        self.pipe = open(js, 'r')

    def getEvent(self):
        msg = []
        while True:
            for char in self.pipe.read(1):
                msg += [ord(char)]
                if len(msg) == 8:
                    event = PyPadEvent(msg)
                    if event.eventType != None:
                        return event
                    msg = []
