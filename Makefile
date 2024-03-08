all: main

CC = clang
override CFLAGS += -pthread

SRC=$(wildcard *.c)
HEADERS=$(wildcard *.h)

main: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -g -o "$@"

main-release: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -Os -s -o "$@"

run: main
	./main

clean:
	rm -f main main-release