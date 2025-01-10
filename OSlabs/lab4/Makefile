CC = gcc
CFLAGS = -Wall -g
TARGET = mychmod

all: $(TARGET)

$(TARGET): mychmod.c
	$(CC) $(CFLAGS) -o $(TARGET) mychmod.c

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
