from array import array
from struct import *
from enum import Enum

class ResponseCode(Enum):
    PROTOCOL_SYNC       = 0x00
    UNKNOWN_COMMAND     = 0x22
    RX_TIMEOUT          = 0xaa
    COMMAND_INTERRUPTED = 0xbb
    COMMAND_SUCCESS     = 0xdd

class CommandCode(Enum):
    GET_STATE          = 1
    GET_VERSION        = 2
    GET_PACKET         = 3
    SEND_PACKET        = 4
    SEND_AND_LISTEN    = 5
    UPDATE_REGISTER    = 6
    RESET              = 7
    LED                = 8
    READ_REGISTER      = 9
    SET_MODE_REGISTERS = 10
    SET_SW_ENCODING    = 11
    SET_PREAMBLE       = 12
    RADIO_RESET_CONFIG = 13

class ByteResponse:
    def __init__(self, response_data):
        self.response_code = ResponseCode(response_data[0])
        self.response_data = bytearray(response_data[1:])

    def __repr__(self):
        return "ByteResponse(%s, %s)" % (self.response_code, self.response_data.hex())


class StringResponse:
    def __init__(self, response_data):
        self.response_code = ResponseCode(response_data[0])
        self.response_data = array('B', response_data[1:])

    def __repr__(self):
        return "StringResponse(\"%s\")" % self.response_data.tostring().decode("UTF-8")

class PacketResponse:
    def __init__(self, response_data):
        self.response_code = ResponseCode(response_data[0])
        if self.response_code == ResponseCode.COMMAND_SUCCESS:
            self.rssi_dec = response_data[1]
            self.packet_num = response_data[2]
            self.response_data = array('B', response_data[3:])

    def __repr__(self):
        if self.response_code != ResponseCode.COMMAND_SUCCESS:
            return "ResponseCode(%s)" % self.response_code
        else:
            hex = "".join(["%02x" % x for x in self.response_data])
            return "PacketResponse(\"%s\")" % hex

class GetStateCommand:
    def data(self):
        return array('B', [CommandCode.GET_STATE.value])

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
        data = pack('>BBL', CommandCode.GET_PACKET.value, self.channel, self.timeout_ms)
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
        conf = pack('>BBBHH', CommandCode.SEND_PACKET.value, self.channel, self.repeat_count, self.delay_ms, self.preamble_extend_ms)
        print("Send packet cmd: %s" % conf.hex())
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
        return array('B', [CommandCode.UPDATE_REGISTER.value, self.register.value, self.value])

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
        return array('B', [CommandCode.SET_SW_ENCODING.value, self.encoding.value])

    def response_type(self):
        return ByteResponse
