# Makefile для компиляции программы с потоками

CC = gcc
CFLAGS = -pthread -Wall -Wextra -std=c99
TARGET = thread_sync

# Все цели
all: $(TARGET)

# Правило для компиляции
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

# Очистка файлов сборки
clean:
	rm -f $(TARGET)
