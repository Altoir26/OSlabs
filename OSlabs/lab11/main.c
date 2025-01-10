#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 10

// Глобальный массив
int shared_array[ARRAY_SIZE];

// Блокировка чтения-записи
pthread_rwlock_t rwlock;

// Индекс записи для пишущего потока
int write_index = 0;

// Функция для пишущего потока
void* writer_thread(void* arg) {
    while (1) {
        pthread_rwlock_wrlock(&rwlock);  // Захват блокировки на запись

        // Записываем текущее значение индекса в массив
        if (write_index < ARRAY_SIZE) {
            shared_array[write_index] = write_index + 1;
            printf("Writer wrote: %d\n", shared_array[write_index]);
            write_index++;
        }

        pthread_rwlock_unlock(&rwlock);  // Освобождение блокировки на запись

        sleep(1);  // Ждем немного перед следующим записыванием
    }

    return NULL;
}

// Функция для читающих потоков
void* reader_thread(void* arg) {
    int tid = *((int*)arg);  // Получаем ID потока

    while (1) {
        pthread_rwlock_rdlock(&rwlock);  // Захват блокировки на чтение

        // Выводим текущее состояние массива
        printf("Reader %d: Array state: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", shared_array[i]);
        }
        printf("\n");

        pthread_rwlock_unlock(&rwlock);  // Освобождение блокировки на чтение

        sleep(1);  // Ждем немного перед следующим чтением
    }

    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[10];
    int reader_ids[10];

    // Инициализация блокировки чтения-записи
    pthread_rwlock_init(&rwlock, NULL);

    // Создаем писательский поток
    pthread_create(&writer, NULL, writer_thread, NULL);

    // Создаем 10 читающих потоков
    for (int i = 0; i < 10; i++) {
        reader_ids[i] = i + 1;  // Идентификатор читающего потока
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    // Ожидаем завершения всех потоков (хотя они не завершатся)
    pthread_join(writer, NULL);
    for (int i = 0; i < 10; i++) {
        pthread_join(readers[i], NULL);
    }

    // Освобождаем ресурсы
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
