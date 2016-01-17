#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 1:
  print "usage: get_rfspy_version.py serialport"
  sys.exit(2)

rl = SerialRfSpy(sys.argv[1])
rl.sync()
rl.send_command(SerialRfSpy.CMD_RESET)

print "OK"
