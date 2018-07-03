from spilink import SPILink
from commands import *
import omni

link = SPILink(debug=True)
link.flush()

omni.configure(link)

packet_data = bytearray.fromhex("000000005800000000fbf9f8fbe0ff1179fed102")
send_cmd = SendPacketCommand(array('B', packet_data), repeat_count=2, preamble_extend_ms=0)
print(link.do_command(send_cmd, timeout=5))
