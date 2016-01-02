#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 2:
  print "usage: get_rfspy_version.py serialport"
  sys.exit(2)

channel = 0

rl = SerialRfSpy(sys.argv[1])
rl.sync()
resp = rl.do_command(SerialRfSpy.CMD_GET_VERSION)

print "Response: " + resp
