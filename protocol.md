# Subg_rfspy protocol


### SPI data polling

The spi version of subg_rfspy acts as an spi slave. To start the exchange, a 0x99 byte is transferred from master, and the received value during that exchange can be ignored. The next byte sent is size of the command that will follow; the received byte indicates how much data is available. After that, all the command data and the data to be received is transferred synchronously.

#### First Exchange:

| master | slave  |
| ------ | ------ |
| 0x99   | Ignore |


#### Second Exchange:

| master    | slave         |
| --------- | ------------- |
| cmd_len   | available_len |


#### Data exchange until MAX(cmd_len, available_len):

| master    | slave        |
| --------- | -----------  |
| cmd[0]    | available[0] |
| cmd[1]    | available[1] |
| cmd[2]    | available[2] |
| ...       | ...          |



## Command List

Different commands can be selected by sending the following numbers through the serial interface.  For types larger than uint8_t, data is sent most significant byte first (msb).

### Interrupt: 0

Interrupts the current command.  Actually, any new command sent during a radio rx operation will interrupt the firmware, and the 0xbb response will be generated.

| data    | size     | Description  |
| ------- | -------- | -----------  |
| 0       | uint8_t  | Command code |

Response: CMD_INTERRUPTED (0xbb)


### Get State: 1

Check to make sure serial communications are functioning. Should return "OK" message.

| data    | size     | Description  |
| ------- | -------- | -----------  |
| 1       | uint8_t  | Command code |

Response: "OK"


### Get Version: 2

Prints the current version of the software.

| data    | size     | Description  |
| ------- | -------- | -----------  |
| 2       | uint8_t  | Command code |

Response: "subg_rfspy 1.0"


### Get Packet: 3

Gets the latest packet of wireless data and displays it on the serial interface.

| data    | size     | Description  |
| ------- | -------- | -----------  |
| 3          | uint8_t  | Command code |
| channel    | uint8_t  | Channel to listen on.|
| timeout_ms | uint32_t | Time to wait before giving up in milliseconds.|

Response: Packet data, or RX_TIMEOUT (0xaa).

### Send Packet: 4

Sends a packet of data from the serial interface over the wireless connection.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 4            | uint8_t | Command code |
| channel      | uint8_t | Channel to send on.|
| repeat_count | uint8_t | Number of times to repeat, after the first send. |
| delay_ms     | uint8_t | Delay between sending packets, in milliseconds. |

Response: Packet data, or RX_TIMEOUT (0xaa).

### Send and Listen: 5

Sends a packet of data from the serial interface over the wireless connection and waits
to receive a packet to display over the serial interface.

| data           | size          | Description  |
| -------------- | ------------- | -----------  |
| 5              | uint8_t       | Command code |
| send_channel   | uint8_t       | Channel to send on.|
| repeat_count   | uint8_t       | Number of times to repeat, after the first send. |
| delay_ms       | uint8_t       | Delay between sending packets, in milliseconds. |
| listen_channel | uint8_t       | Channel to listen on.|
| timeout_ms     | uint32_t      | Time to wait before giving up in milliseconds.|
| retry_count    | uint8_t       | Number of times to retry the send, if nothing is received.|
| packet_data    | 0..packet_len | Raw packet data. Length is specified at the framing (spi) layer. |

### Update Register: 6

Use this command to update one of the major registers with a desired value. See section Major Registers for a list of available registers and their functions.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 6            | uint8_t | Command code |
| register     | uint8_t | Register to update. |
| value        | uint8_t | Value to set register to. |

Returns 1 on success, 2 on invalid register.

### Reset: 7

Use this command to reset the device.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 7            | uint8_t | Command code |

No response; client should wait, and then check status after 100ms or so.

### LED: 8

Use this command to manually control the LEDs on the device.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 8            | uint8_t | Command code |
| led_num      | uint8_t | LED number (0 = green, 1 = blue) |
| led_mode     | uint8_t | LED mode (0 = off, 1 = on, 2 = auto) |

### Read Register: 9

Use this command to read the current value of one of the major registers. See Major Registers section
for a list of available registers and their functions.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 9            | uint8_t | Command code |
| register     | uint8_t | Register to read. |

Returns the value of the register, or 0x5a if an invalid register was specified.

### Set Mode Registers: 10

This command allows you to set specific values for registers during only rx or tx.

| data         | size    | Description  |
| ------------ | ------- | -----------  |
| 10           | uint8_t | Command code |
| mode         | uint8_t | Mode to set registers for (tx = 1, rx = 2) |
| count        | uint8_t | Number of registers that will follow |
| register1    | uint8_t | First register to set |
| value1       | uint8_t | Value to set for first register |
| register2    | uint8_t | Second register to set |
| value2       | uint8_t | Value to set for second register |
| ...          | ...     | ... |

Returns 0.

### Set Software Encoding: 11

In addition to the native encodings that the cc111x hardware can support, there is support for additional encodings built in at the software/firmware layer.  

_Note_: The manchester support differs from the hardware based manchester support by being able to detect manchester errors and mark the end of packet properly.  If using software based manchester support, hardware based manchester should not be turned on (i.e. MDMCFG2.MANCHESTER_EN=0). The bitrate should be set at 2x the decoded packet data rate.

| data          | size    | Description  |
| ------------- | ------- | -----------  |
| 11            | uint8_t | Command code |
| encoding_type | uint8_t | Encoding type (0:none, 1:manchester, 2:4b6b)
