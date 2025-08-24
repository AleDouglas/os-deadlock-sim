CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2 -Iinclude -Iinclude -D_POSIX_C_SOURCE=200809L
SRCS    = src/main.c src/process.c src/simulator.c src/stubs.c src/ostrich.c src/banker.c src/detector.c src/dispatcher.c src/logger.c
BIN     = os-deadlock-sim

all: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN)

clean:
	rm -f $(BIN)
