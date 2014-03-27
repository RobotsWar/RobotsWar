#!/usr/bin/python

import socket, threading, time, mutex, random

"""
    Remotely control the terminal
"""
class TerminalControl:
    def __init__(self, com, frequency = 50):
        self.com = com;
        self.running = True
        self.com.connect()
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
            self.com.connect()

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
        if not self.com.connected and self.running:
            self.com.connect()
        if self.com.connected:
            self.com.send(command)
        del self.queue[key]

    def send(self, key, *values):
        self.mutex.acquire()
        self.queue[key] = values;
        self.mutex.release()

