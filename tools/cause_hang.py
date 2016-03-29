#!/usr/bin/env python

from serial_rf_spy import SerialRfSpy
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 1:
  print "usage: test.py serialport"
  sys.exit(2)

serial_device = sys.argv[1]

rl = SerialRfSpy(serial_device)
rl.sync()
params = bytearray("00000000000033e800a968e55658e568d555d26000".decode('hex'))
rl.send_command(SerialRfSpy.CMD_SEND_AND_LISTEN, params)
params = bytearray("0000000a50".decode('hex'))
rl.send_command(SerialRfSpy.CMD_GET_PACKET, params)
resp = rl.get_response()
print "resp1 = %s" % str(resp).encode('hex')
resp = rl.get_response()
print "resp2 = %s" % str(resp).encode('hex')

