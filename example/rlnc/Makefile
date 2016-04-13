EXEC = rlnc

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) -o $@ $^ $(OBJECT) -I$(INCLUDE)

clean:
	rm -rf $(EXEC)
