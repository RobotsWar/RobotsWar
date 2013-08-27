#!/usr/bin/python

import time

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
        self.js = js
        self.open()
        self.bindings = {}

    def open(self):
        self.connected = False
        while not self.connected:
            try:
                print('JOYPAD: Opening %s' % self.js)
                self.pipe = open(self.js, 'r')
                self.connected = True
                print('JOYPAD: Opened')
            except:
                self.connected = False
                time.sleep(0.3)
                print('JOYPAD: %s Not found, retrying...' % self.js)

    def appendCallback(self, key, callback):
        if not key in self.bindings:
            self.bindings[key] = []

        self.bindings[key] += [callback]

    def onButtonPressed(self, index, callback):
        self.appendCallback('button/%d/1' % index, callback)
    
    def onButtonReleased(self, index, callback):
        self.appendCallback('button/%d/0' % index, callback)
   
    def onAxis(self, index, callback):
        self.appendCallback('axis/%d' % index, callback)

    def process(self, event):
        if event.eventType == 'button':
            key = 'button/%d/%d' % (event.index, event.value)
        if event.eventType == 'axis':
            key = 'axis/%d' % event.index

        if key in self.bindings:
            callbacks = self.bindings[key]
            for callback in callbacks:
                if event.eventType == 'button':
                    callback()
                else:
                    callback(event.value)

    def getEvent(self):
        msg = []
        while True:
            try:
                for char in self.pipe.read(1):
                    msg += [ord(char)]
                    if len(msg) == 8:
                        event = PyPadEvent(msg)
                        if event.eventType != None:
                            self.process(event)
                            return event
                        msg = []
            except KeyboardInterrupt:
                raise
            except:
                print('JOYPAD: Disconnected')
                self.open()
                msg = []
