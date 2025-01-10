CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = fork_example

all: $(TARGET)

$(TARGET): fork_example.c
	$(CC) $(CFLAGS) -o $(TARGET) fork_example.c

clean:
	rm -f $(TARGET)
