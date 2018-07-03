from spilink import SPILink
from commands import *
from array import array
import omni

link = SPILink(debug=True)
link.flush()

omni.configure(link)

# packet_data = [1,2,3,4,5,6,7,8,9]
# send_cmd = SendPacketCommand(array('B', packet_data), repeat_count=2, preamble_extend_ms=0)
# print(link.do_command(send_cmd, timeout=5))

while True:
    print(link.do_command(GetPacketCommand(timeout_ms=50000), timeout=51))
