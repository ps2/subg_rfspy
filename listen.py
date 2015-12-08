#!/usr/bin/python

import serial
import binascii
import time
from datetime import datetime
import sys

CMD_GET_STATE = 1
CMD_GET_VERSION = 2
CMD_SET_CHANNEL = 3
CMD_GET_PACKET = 4

channel = 1

class SerialRL:

  def __init__(self, serial_port):
    self.ser = serial.Serial(serial_port, 19200, rtscts=1, timeout=1)
    self.buf = bytearray()

  def send_command(self, command, param=""): 
    self.ser.write(chr(command))
    if len(param) > 0:
      self.ser.write(param)

  def get_response(self, timeout=0): 
    start = time.time()
    while 1:
      bytesToRead = self.ser.inWaiting()
      if bytesToRead > 0:
        self.buf.extend(self.ser.read(bytesToRead))
      eop = self.buf.find(b'\x00',2)
      if eop > 0:
        r = self.buf[:eop]
        del self.buf[:(eop+1)]
        return r
      if (timeout > 0) and (start + timeout < time.time()):
        return bytearray()
      time.sleep(0.005)
  
  def sync(self):
    while 1:
      self.send_command(CMD_GET_STATE) 
      data = self.get_response(1)
      if data == "OK":
        print "RileyLink " + data
        break 
 
    while 1:
      self.send_command(CMD_GET_VERSION) 
      data = self.get_response(1)
      if len(data) >= 3:
        print "Version: " + data
        break 


if len(sys.argv) < 2:
  print "usage: listen.py serialport [channel]"
  sys.exit(2)

channel = 2

if len(sys.argv) == 3:
  channel = int(sys.argv[2])

rl = SerialRL(sys.argv[1])
rl.sync()
rl.send_command(CMD_SET_CHANNEL, chr(channel))
while 1:
  rl.send_command(CMD_GET_PACKET)
  packet = rl.get_response()
  print datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3] + ": " + binascii.hexlify(packet)

