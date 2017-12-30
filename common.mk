RADIO_LOCALE := US
ifeq ($(RADIO_LOCALE),US)
	RADIO_LOCALE_DEF := -DUS_RADIO_LOCALE
endif

TARGET_DEVICE ?= CC1110

CODE_LOC := 0x000
CODE_LOC_NAME := STDLOC

TARGET_BUILD := ${SERIAL_TYPE}_${BOARD_TYPE}_${RADIO_LOCALE}_${CODE_LOC_NAME}

CC=sdcc

LDFLAGS=--xram-loc 0xf000 --xram-size 0x1000 --code-loc ${CODE_LOC} --code-size 0x8000
CFLAGS=-I. -I${SERIAL_TYPE} --verbose ${RADIO_LOCALE_DEF} -D${BOARD_TYPE} ${BOARD_PARAMS} ${SERIAL_PARAMS}

default: output output/${TARGET_BUILD} output/${TARGET_BUILD}/${TARGET_BUILD}.hex

common_modules = radio.rel main.rel timer.rel encoding.rel manchester.rel \
	           fourbsixb.rel commands.rel delay.rel hardware.rel

clean:
	rm -rf output/${TARGET_BUILD}

%.rel: %.c
	$(CC) $(CFLAGS) -o output/${TARGET_BUILD}/$@ -c $<

# The serial implementation used depends on the TARGET_BUILD parameter, so is built
# separately from the other .rel files
serial.rel: ${SERIAL_TYPE}/serial.c
	# ${REL}
	$(CC) $(CFLAGS) -o output/${TARGET_BUILD}/$@ -c $< $(REL)

output/${TARGET_BUILD}/${TARGET_BUILD}.hex: $(common_modules) $(REL) serial.rel
	cd output/${TARGET_BUILD} && $(CC) $(LDFLAGS) $(CFLAGS) $(common_modules) $(REL) serial.rel  -o ${TARGET_BUILD}.hex

install: output/${TARGET_BUILD} output/${TARGET_BUILD}/${TARGET_BUILD}.hex
	sudo cc-tool -n ${TARGET_DEVICE} --log install.log -ew output/${TARGET_BUILD}/${TARGET_BUILD}.hex

test: main.c output
	gcc -g -o output/${TARGET_BUILD}/test -DNON_NATIVE_TEST main.c

output:
	mkdir output

output/${TARGET_BUILD}: output
	mkdir -p output/${TARGET_BUILD}/

output/${TARGET_BUILD}/%.rel : %.c Makefile
