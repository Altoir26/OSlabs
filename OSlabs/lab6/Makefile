CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = pipe_fifo

all: $(TARGET)

$(TARGET): pipe_fifo.c
	$(CC) $(CFLAGS) -o $(TARGET) pipe_fifo.c

clean:
	rm -f $(TARGET)
	rm -f /tmp/my_fifo
