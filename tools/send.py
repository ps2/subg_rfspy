#!/usr/bin/env python

from serial_rl import SerialRL
import sys
import binascii
import time
from datetime import datetime

if len(sys.argv) < 2:
  print "usage: send.py serialport [channel]"
  sys.exit(2)

channel = 2

if len(sys.argv) == 3:
  channel = int(sys.argv[2])

rl = SerialRL(sys.argv[1])
rl.sync()
rl.do_command(SerialRL.CMD_SET_CHANNEL, chr(channel))

for _ in range(50):
  rl.do_command(SerialRL.CMD_SEND_PACKET, binascii.unhexlify("0000a968e55658e594d555d1a500"))
