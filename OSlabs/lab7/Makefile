# Makefile

CC = gcc
CFLAGS = -Wall -g

# Исходные файлы
SENDER_SRC = sender.c
RECEIVER_SRC = receiver.c

# Исполняемые файлы
SENDER_BIN = sender
RECEIVER_BIN = receiver

# Правила для сборки
all: $(SENDER_BIN) $(RECEIVER_BIN)

$(SENDER_BIN): $(SENDER_SRC)
	$(CC) $(CFLAGS) -o $(SENDER_BIN) $(SENDER_SRC) -lrt

$(RECEIVER_BIN): $(RECEIVER_SRC)
	$(CC) $(CFLAGS) -o $(RECEIVER_BIN) $(RECEIVER_SRC) -lrt

# Очистка
clean:
	rm -f $(SENDER_BIN) $(RECEIVER_BIN)
