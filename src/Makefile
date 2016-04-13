all: $(patsubst %.c,%.o,$(wildcard *.c))

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $(BUILD)/$@ -I$(INCLUDE)
