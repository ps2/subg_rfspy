#!/usr/bin/env python

import os
import serial
import time

class SerialRfSpy:

  CMD_GET_STATE = 1
  CMD_GET_VERSION = 2
  CMD_GET_PACKET = 3
  CMD_OLD_SEND_PACKET = 4
  CMD_SEND_AND_LISTEN = 5
  CMD_UPDATE_REGISTER = 6
  CMD_RESET = 7
  CMD_SEND_PACKET = 8

  def __init__(self, serial_port, rtscts=None):
    if not rtscts:
      rtscts = int(os.getenv('RFSPY_RTSCTS', 1))
    if serial_port.find('spi') >= 0:
        import spi_serial
        self.ser = spi_serial.SpiSerial()
    else:
        self.ser = serial.Serial(serial_port, 19200, rtscts=rtscts, timeout=1)
    self.buf = bytearray()

  def do_command(self, command, param=""):
    self.send_command(command, param)
    return self.get_response()

  def send_command(self, command, param=""):
    cmd_str = chr(command) + param
    self.ser.write(cmd_str)

  def get_response(self, timeout=0):
    start = time.time()
    while 1:
      bytesToRead = self.ser.inWaiting()
      if bytesToRead > 0:
        self.buf.extend(self.ser.read(bytesToRead))
      eop = self.buf.find(b'\x00',0)
      if eop >= 0:
        r = self.buf[:eop]
        del self.buf[:(eop+1)]
        return r
      if (timeout > 0) and (start + timeout < time.time()):
        return bytearray()
      time.sleep(0.005)

  def sync(self):
    while 1:
      self.send_command(self.CMD_GET_STATE)
      data = self.get_response(1)
      if data == "OK":
        print "RileyLink " + data
        break
      print "retry", len(data), str(data).encode('hex'), "(Do you need to run 'export RFSPY_RTSCTS=0' first?)"

    while 1:
      self.send_command(self.CMD_GET_VERSION)
      data = self.get_response(1)
      if len(data) >= 3:
        print "Version: " + data
        break
      print "retry", len(data), str(data).encode('hex'), "(Do you need to run 'export RFSPY_RTSCTS=0' first?)"
