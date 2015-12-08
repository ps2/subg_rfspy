
default: output output/subg_rfspy.hex
CC=sdcc
CFLAGS=-I. --verbose -DRILEYLINK
LDFLAGS=--xram-loc 0xf000 --xram-size 0x1000
output:
	mkdir output

output/%.rel : %.c
	$(CC) $(CFLAGS) -o output/ -c $<

relfiles = output/radio.rel output/serial.rel output/main.rel output/timer.rel output/commands.rel

output/subg_rfspy.hex: $(relfiles)
	$(CC) $(LDFLAGS) $(CFLAGS) $(relfiles) -o output/subg_rfspy.hex

install: output/subg_rfspy.hex
	cc-tool -n cc1110 --log install.log -ew output/subg_rfspy.hex

test: main.c output
	gcc -g -o output/test -DNON_NATIVE_TEST main.c
