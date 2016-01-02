#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 2:
  print "usage: send.py serialport [channel]"
  sys.exit(2)

channel = 0

if len(sys.argv) == 3:
  channel = int(sys.argv[2])

rl = SerialRfSpy(sys.argv[1])
rl.sync()
rl.do_command(SerialRfSpy.CMD_SET_CHANNEL, chr(channel))

for _ in range(80):
  rl.do_command(SerialRfSpy.CMD_SEND_PACKET, binascii.unhexlify("0000a968e55658e594d555d1a500"))
