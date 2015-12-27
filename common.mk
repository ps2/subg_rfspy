TARGET_BUILD := ${SERIAL_TYPE}_${BOARD_TYPE}

CC=sdcc

LDFLAGS=--xram-loc 0xf000 --xram-size 0x1000
CFLAGS=-I. -I${SERIAL_TYPE} --verbose -D${BOARD_TYPE} ${BOARD_PARAMS} ${SERIAL_PARAMS}

default: output output/${TARGET_BUILD} output/${TARGET_BUILD}/${TARGET_BUILD}.hex

common_modules = radio.rel main.rel timer.rel \
	           commands.rel delay.rel

clean:
	rm -rf output/${TARGET_BUILD}

%.rel: %.c
	$(CC) $(CFLAGS) -o output/${TARGET_BUILD}/$@ -c $<

# The serial implementation used depends on the TARGET_BUILD parameter, so is built
# separately from the other .rel files
serial.rel: ${SERIAL_TYPE}/serial.c
	$(CC) $(CFLAGS) -o output/${TARGET_BUILD}/$@ -c $<

output/${TARGET_BUILD}/${TARGET_BUILD}.hex: $(common_modules) serial.rel
	cd output/${TARGET_BUILD} && $(CC) $(LDFLAGS) $(CFLAGS) $(common_modules) serial.rel -o ${TARGET_BUILD}.hex

install: output/${TARGET_BUILD} output/${TARGET_BUILD}/${TARGET_BUILD}.hex
	cc-tool -n cc1110 --log install.log -ew output/${TARGET_BUILD}/${TARGET_BUILD}.hex

test: main.c output
	gcc -g -o output/${TARGET_BUILD}/test -DNON_NATIVE_TEST main.c

output:
	mkdir output

output/${TARGET_BUILD}:
	mkdir output/${TARGET_BUILD}/

output/${TARGET_BUILD}/%.rel : %.c Makefile
