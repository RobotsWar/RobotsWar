#!/usr/bin/python

import socket, threading, time, mutex, random

"""
    Remotely control the terminal
"""
class TerminalControl:
    def __init__(self, host, frequency = 50):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(host)
        self.mutex = threading.Lock()
        self.queue = {}
        self.frequency = frequency
        self.running = True
        self.thread = threading.Thread(None, self.DispatchThread, None, ())
        self.thread.start()

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
        self.doSend(command)
        del self.queue[key]

    def doSend(self, command):
        self.socket.send(command)

    def send(self, key, *values):
        self.mutex.acquire()
        self.queue[key] = values;
        self.mutex.release()

