CC=gcc
CFLAGS=-O2 -march=native -Wall -Wextra
BIN=build/sum2d

all: $(BIN) 

$(BIN): src/sum2d.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $<

profile: $(BIN)
	taskset -c 0 ./sum2d --min 256 --max 8192 --factor 2 --reps 8 | column -t -s,

clean:
	rm -rf build

