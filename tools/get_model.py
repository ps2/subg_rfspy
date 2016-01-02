#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 2:
  print "usage: get_model.py serialport"
  sys.exit(2)

channel = 0

rl = SerialRfSpy(sys.argv[1])
rl.sync()
rl.do_command(SerialRfSpy.CMD_SET_CHANNEL, chr(0))

resp = rl.do_command(SerialRfSpy.CMD_SEND_PACKET, binascii.unhexlify("0000a968e55658e568d555d26500"))
rl.do_command(SerialRfSpy.CMD_SET_CHANNEL, chr(2))
packet = rl.do_command(SerialRfSpy.CMD_GET_PACKET, chr(0))

print "Response: " + packet
