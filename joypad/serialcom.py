#!/usr/bin/python

import serial, threading, time, mutex, random

"""
    Remotely control the terminal
"""
class SerialCom:
    def __init__(self, device, baudrate = 115200):
        self.remote = (device, baudrate)

    def disconnect(self):
        self.serial.close()
        self.connected = False

    def connect(self):
        try:
            print('ROBOT: Connecting to %s, baudrate=%d...' % self.remote)
            self.serial = serial.Serial(self.remote[0], self.remote[1], timeout=0)
            self.connected = True
            print('ROBOT: Connected')
        except:
            self.connected = False
            print('ROBOT: Unable to open port %s, baudrate=%d' % self.remote)

    def send(self, command):
        try:
            self.serial.write(command)
        except:
            print('ROBOT: Connection closed')
            self.connected = False

    def flush(self):
        try:
            while self.serial.read() != '':
                pass
        except:
            self.connected = False
