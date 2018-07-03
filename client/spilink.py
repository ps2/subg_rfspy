from pyftdi.spi import SpiController
import time
from commands import *

RILEYLINK_XTAL_FREQUENCY = 24000000

class SPITimeout(Exception):
    pass

class SPILink:
    BIT_REV = [int('{:08b}'.format(n)[::-1], 2) for n in range(0,256)]

    def __init__(self, device='ftdi://ftdi:232h/1', debug=False):
        self.device_str = device
        self.debug = debug
        self.ctrl = SpiController(silent_clock=False)
        self.ctrl.configure(device, cs_count=1)
        self.spi = self.ctrl.get_port(cs=0, freq=100000)
        self.gpio = self.ctrl.get_gpio()
        direction = self.gpio.direction
        self.gpio.set_direction(0x30, 0x10) # Add reset as output
        self.gpio.write(0x10)

    def reset(self):
        self.gpio.write(0x00) # Pull reset line low
        time.sleep(0.1)
        self.gpio.write(0x10) # Reset line high
        time.sleep(0.8) # Wait for cc111x to boot

    def flush(self):
        # Send interrupt command
        _, available = self.exchange([0x99, 1], readlen=2)
        data = self.exchange([], readlen=max(1,available))
        # Discard any remaining data
        _, available = self.exchange([0x99, 0], readlen=2)
        while (available > 0):
            data = self.exchange([], readlen=available)
            if self.debug:
                print("discarding %s" % "".join(["%02x" % x for x in data]))
            _, available = self.exchange([0x99, 0], readlen=2)

    def reverse_bits(self, data):
        return [SPILink.BIT_REV[x] for x in data]

    def as_hex(self, data):
        return "".join(["%02x" % x for x in data])

    def exchange(self, data, readlen=0):
        lsb_send = self.reverse_bits(data)
        lsb_rcv = self.spi.exchange(lsb_send,readlen=readlen, duplex=True)
        return self.reverse_bits(lsb_rcv)

    def send_command(self, command):
        data = command.data()
        if len(data) == 0:
            print("Empty command!")
            return

        if self.debug:
            print("send: %s" % self.as_hex(data))
        response = None
        _, available = self.exchange([0x99, len(data)], readlen=2)
        if available > 0:
            print("Unexpected data available from client while sending command.")
            return
        self.exchange(data, readlen=0)

    def wait_for_response(self, response_type, timeout=1):
        end_time = time.time() + timeout
        response = None
        while(time.time() < end_time and response == None):
            _, available = self.exchange([0x99, 0], readlen=2)
            if (available > 0):
                if self.debug:
                    print("response: available = %d" % available)
                rdata = self.exchange([], readlen=available)
            else:
                time.sleep(0.01)
            if available > 0:
                response = rdata
            data = []

        if response == None:
            raise SPITimeout()
        if self.debug:
            print("recv: %s" % self.as_hex(response))
        response_obj = response_type(response)
        return response_obj

    def do_command(self, command, timeout=1):
        self.send_command(command)
        response = self.wait_for_response(command.response_type(), timeout)
        if response.response_code != ResponseCode.COMMAND_INTERRUPTED:
            return response
        else:
            print("Command interrupted...")
            return self.wait_for_response(command.response_type(), timeout)

    def update_register(self, register, value):
        return self.do_command(UpdateRegisterCommand(register, value))

    def set_base_frequency(self, freq):
        val = int(freq / (RILEYLINK_XTAL_FREQUENCY / pow(2,16)))
        self.update_register(Register.FREQ0, val & 0xff)
        self.update_register(Register.FREQ1, (val >> 8) & 0xff)
        self.update_register(Register.FREQ2, (val >> 16) & 0xff)
