#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 3:
  print "usage: get_rfspy_version.py serialport addr value"
  print "Both addr and value should be specified with hex, starting with 0x*"
  sys.exit(2)

serial_device = sys.argv[1]
address = int(sys.argv[2], 16)
value = int(sys.argv[3], 16)

rl = SerialRfSpy(serial_device)
rl.sync()
resp = rl.do_command(SerialRfSpy.CMD_UPDATE_REGISTER, chr(address) + chr(value))

if ord(resp) == 1:
  print "SUCCESS"
else:
  print "ERROR - %i" % ord(resp)
