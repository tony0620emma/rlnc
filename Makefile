CFLAGS = -g -O2 -Wall

all: main

main: main.c decoder.c encoder.c msb.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rf main
