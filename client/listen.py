from pyftdi.spi import SpiController
from spilink import SPILink
from commands import *
from array import array
import time

link = SPILink(debug=True)
link.flush()

print(link.do_command(SetSoftwareEncoding(Encoding.FOURBSIXB)))

while True:
    print(link.do_command(GetPacketCommand(timeout_ms=50000), timeout=51))
    time.sleep(0.1)
