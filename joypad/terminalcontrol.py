#!/usr/bin/python

import socket, threading, time, mutex, random

"""
    Remotely control the terminal
"""
class TerminalControl:
    def __init__(self, host, frequency = 50):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.running = True
        self.host = host
        self.connect();
        self.mutex = threading.Lock()
        self.queue = {}
        self.frequency = frequency
        self.thread = threading.Thread(None, self.DispatchThread, None, ())
        self.thread.start()

    def disconnect(self):
        print('ROBOT: Disconnecting')
        self.socket.close();

    def connect(self):
        if self.running:
            try:
                print('ROBOT: Connecting to %s:%d...' % self.host)
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.connect(self.host)
                self.connected = True
                print('ROBOT: Connected')
            except:
                self.connected = False
                print('ROBOT: Unable to connect to %s:%d' % self.host)

    def DispatchThread(self):
        while self.running:
            self.processOneItem()
            time.sleep(1.0/self.frequency)

    def processOneItem(self):
        self.mutex.acquire()
        keys = self.queue.keys()
        if len(keys):
            choice = random.choice(keys)
            self.process(choice)
        self.mutex.release()

    def process(self, key):
        values = self.queue[key]
        command = "%s %s\n" % (key, ' '.join(map(str,list(values))))
        if not self.connected and self.running:
            self.connect()
        if self.connected:
            self.doSend(command)
        del self.queue[key]

    def doSend(self, command):
        try:
            self.socket.send(command)
        except:
            print('ROBOT: Connection closed')
            self.connected = False

    def send(self, key, *values):
        self.mutex.acquire()
        self.queue[key] = values;
        self.mutex.release()

