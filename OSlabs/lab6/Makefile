CC = gcc
CFLAGS = -Wall -std=c99

TARGET = pipe_fifo

all: $(TARGET)

$(TARGET): pipe_fifo.c
	$(CC) $(CFLAGS) -o $(TARGET) pipe_fifo.c

clean:
	rm -f $(TARGET) $(FIFO_NAME)
