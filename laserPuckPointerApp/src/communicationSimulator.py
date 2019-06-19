# This script simulates the laserPuckPointer hardware to allow testing of the support module and
# streamDevice. Run this on a local machine and point the AsynIP port to localhost:8000.

import socket
from threading import *
import re
import array as arr

serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "localhost"
port = 8000
print (host)
print (port)
serversocket.bind((host, port))

class client(Thread):
    def __init__(self, socket, address):
        Thread.__init__(self)
        self.sock = socket
        self.addr = address
        self.lastCommand = ""
        self.start()
        self.data = {
            "P":101,
            "T":102,
            "M":'R',
            "L":0
        }

    def run(self):
        while 1:
            buffer = self.sock.recv(1024)
            self.lastCommand+=buffer
            # Check if we get a LF to signify end of command
            if str(buffer[-1:]) == '\n':
                #If it's a read
                if '?' in self.lastCommand:
                    print "Received command = "+self.lastCommand
                    print "Response = " + str(self.data[self.lastCommand[0]])
                    self.sock.send(str(self.data[self.lastCommand[0]])+"\r\n")
                    self.lastCommand = ""
                else:
                    # It must be a write
                    if self.lastCommand[1].isdigit():
                        value = re.findall("\d+",self.lastCommand)[0]
                    else:
                        value = self.lastCommand[1]

                    print "Set "+str(value)
                    self.data[self.lastCommand[0]] = value
                    self.lastCommand = ""



serversocket.listen(5)
print ('server started and listening')
while 1:
    clientsocket, address = serversocket.accept()
    client(clientsocket, address)
