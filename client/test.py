from pyftdi.spi import SpiController
from spilink import SPILink
from commands import *
from array import array
import time

link = SPILink(debug=True)
link.reset() # This doesn't work when cc-debugger is connected
#link.flush()

print("GetState:", link.do_command(GetStateCommand()))
print("GetVersion:", link.do_command(GetVersionCommand()))

print("SetSoftwareEncoding:", link.do_command(SetSoftwareEncoding(Encoding.FOURBSIXB)))

#packet_data = bytes.fromhex("a965a5d1a8da65a5665715555555555555")
packet_data = [1,2,3,4,5,6,7,8,9]
send_cmd = SendPacketCommand(array('B', packet_data), repeat_count=2, preamble_extend_ms=1)
print("SendPacket:", link.do_command(send_cmd, timeout=5))


# Issue a longer listening command, and interrupt it
link.send_command(GetPacketCommand(timeout_ms=50000))
time.sleep(0.1)
print("SetSoftwareEncoding:", link.do_command(SetSoftwareEncoding(Encoding.FOURBSIXB)))
print("GetVersionCommand:", link.do_command(GetVersionCommand()))
