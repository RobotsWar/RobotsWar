#!/usr/bin/python

import socket, threading, time, mutex, random

"""
    Remotely control the terminal
"""
class SocketCom:
    def __init__(self, host, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = (host, port)

    def disconnect(self):
        self.socket.close()
        self.connected = False

    def connect(self):
        try:
            print('ROBOT: Connecting to %s:%d...' % self.host)
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect(self.host)
            self.connected = True
            print('ROBOT: Connected')
        except:
            self.connected = False
            print('ROBOT: Unable to connect to %s:%d' % self.host)

    def send(self, command):
        try:
            self.socket.send(command)
        except:
            print('ROBOT: Connection closed')
            self.connected = False

    def close(self):
        self.socket.close()
        self.connected = False

    def flush(self):
        self.socket.setblocking(0)
        try:
            while self.socket.recv(1) != '':
                pass
        except:
            pass
