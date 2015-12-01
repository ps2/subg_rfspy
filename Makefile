
default: output/subg_rfspy.hex
CC=sdcc
CFLAGS=-I. --verbose
output:
	mkdir output

output/main.rel: main.c
	$(CC) $(CFLAGS) -c main.c -o output/
output/subg_rfspy.hex: output/main.rel
	$(CC) $(CFLAGS) output/main.rel -o output/subg_rfspy.hex
install: output/subg_rfspy.hex
	cc-tool -n cc1110 --log install.log -ew output/subg_rfspy.hex

test: main.c output
	gcc -g -o output/test -DNON_NATIVE_TEST main.c
