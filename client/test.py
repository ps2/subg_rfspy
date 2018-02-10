from pyftdi.spi import SpiController
from spilink import SPILink
from commands import *
from array import array


link = SPILink()
link.reset() # This doesn't work when cc-debugger is connected
link.flush()

print(link.do_command(GetStateCommand()))
print(link.do_command(GetVersionCommand()))

print(link.do_command(SetSoftwareEncoding(Encoding.FOURBSIXB)))

#packet_data = bytes.fromhex("a965a5d1a8da65a5665715555555555555")
packet_data = [1,2,3,4,5,6,7,8,9]
send_cmd = SendPacketCommand(array('B', packet_data), repeat_count=2, preamble_extend_ms=10)
print(link.do_command(send_cmd, timeout=5))

print(link.do_command(GetPacketCommand(timeout_ms=50000), timeout=50))
