CC = gcc
CFLAGS = -Wall -Wextra -O2

all: mycat mygrep

mycat: mycat.c
	$(CC) $(CFLAGS) -o mycat mycat.c

mygrep: mygrep.c
	$(CC) $(CFLAGS) -o mygrep mygrep.c

clean:
	rm -f mycat mygrep
