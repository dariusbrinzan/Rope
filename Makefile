CC=gcc
EXECUTABLE_NAME=exec
CFLAGS=-g -Wall -Wextra
FILE_NAME=rope

# Execute call
TARGETS: rope_file

# First call: rope compilation
rope_file: $(FILE_NAME).h utils.h
	$(CC) -o $(EXECUTABLE_NAME) $(FILE_NAME).c $(CFLAGS)

clean:
	rm $(EXECUTABLE_NAME)

.PHONY: build rope clean