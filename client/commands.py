from array import array
from struct import *
from enum import Enum

class ErrorCode(Enum):
    PROTOCOL_SYNC       = 0x00
    RX_TIMEOUT          = 0xaa
    COMMAND_INTERRUPTED = 0xbb


class ByteResponse:
    def __init__(self, response_data):
        if len(response_data) > 1:
            raise ValueError
        self.response_byte =  array('B', response_data)[0]

    def __repr__(self):
        return "ByteResponse(\"%d\")" % self.response_byte


class StringResponse:
    def __init__(self, response_data):
        self.response_data = array('B', response_data)

    def __repr__(self):
        return "StringResponse(\"%s\")" % self.response_data.tostring().decode("UTF-8")

class PacketResponse:
    def __init__(self, response_data):
        self.error_code = None
        if len(response_data) < 2:
            self.error_code = ErrorCode(response_data[0])
        else:
            self.rssi_dec = response_data[0]
            self.packet_num = response_data[1]
            self.response_data = array('B', response_data[2:])

    def __repr__(self):
        if self.error_code:
            return "PacketErrorResponse(%s)" % self.error_code.name
        else:
            hex = "".join(["%02x" % x for x in self.response_data])
            return "PacketResponse(\"%s\")" % hex

class GetStateCommand:
    def data(self):
        return array('B', [1])

    def response_type(self):
        return StringResponse

class GetVersionCommand:
    def data(self):
        return array('B', [2])

    def response_type(self):
        return StringResponse

class GetPacketCommand:
    def __init__(self, timeout_ms=100, channel=0):
        self.channel = channel
        self.timeout_ms = timeout_ms

    def data(self):
        data = pack('>BBL', 3, self.channel, self.timeout_ms)
        return array('B', data)

    def response_type(self):
        return PacketResponse

class SendPacketCommand:
    def __init__(self, packet_data, repeat_count=0, delay_ms=5, preamble_extend_ms=0, channel=0):
        self.channel = channel
        self.repeat_count = repeat_count
        self.delay_ms = delay_ms
        self.preamble_extend_ms = preamble_extend_ms
        self.packet_data = packet_data

    def data(self):
        conf = pack('>BBBHH', 4, self.channel, self.repeat_count, self.delay_ms, self.preamble_extend_ms)
        return array('B', conf) + self.packet_data

    def response_type(self):
        return ByteResponse

class Register(Enum):
    SYNC1    = 0x00
    SYNC0    = 0x01
    PKTLEN   = 0x02
    PKTCTRL1 = 0x03
    PKTCTRL0 = 0x04
    FSCTRL1  = 0x07
    FREQ2    = 0x09
    FREQ1    = 0x0a
    FREQ0    = 0x0b
    MDMCFG4  = 0x0c
    MDMCFG3  = 0x0d
    MDMCFG2  = 0x0e
    MDMCFG1  = 0x0f
    MDMCFG0  = 0x10
    DEVIATN  = 0x11
    MCSM0    = 0x14
    FOCCFG   = 0x15
    AGCCTRL2 = 0x17
    AGCCTRL1 = 0x18
    AGCCTRL0 = 0x19
    FREND1   = 0x1a
    FREND0   = 0x1b
    FSCAL3   = 0x1c
    FSCAL2   = 0x1d
    FSCAL1   = 0x1e
    FSCAL0   = 0x1f
    TEST1    = 0x24
    TEST0    = 0x25
    PATABLE0 = 0x2e

class UpdateRegisterCommand:
    def __init__(self, register, value):
        self.register = register
        self.value = value

    def data(self):
        return array('B', [6, self.register.value, self.value])

    def response_type(self):
        return ByteResponse


class Encoding(Enum):
    NONE = 0
    MANCHESTER = 1
    FOURBSIXB = 2

class SetSoftwareEncoding:
    def __init__(self, encoding):
        self.encoding = encoding

    def data(self):
        return array('B', [11, self.encoding.value])

    def response_type(self):
        return ByteResponse
