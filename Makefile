CFLAGS = -g -O2 -Wall -msse2 --std gnu99

all: main

main: main.c decoder.c encoder.c msb.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf main
