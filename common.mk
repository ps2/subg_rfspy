
CC=sdcc

default: output output/subg_rfspy.hex

output:
	mkdir output

output/%.rel : %.c Makefile

%.rel: %.c
	$(CC) $(CFLAGS) -o output/$@ -c $<

common_modules = radio.rel main.rel timer.rel \
           commands.rel delay.rel

clean:
	rm -rf output 
