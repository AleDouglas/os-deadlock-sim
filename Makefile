CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -Iinclude
SRCS    = src/main.c src/process.c src/simulator.c src/stubs.c
BIN     = os-deadlock-sim

all: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN)

clean:
	rm -f $(BIN)
