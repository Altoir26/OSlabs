CC = gcc
CFLAGS = -pthread -Wall

# Имя выходного файла
TARGET = thread_rwlock

# Исходный файл
SRC = main.c

# Правила для компиляции и линковки
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

# Правило для очистки
clean:
	rm -f $(TARGET)
