#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 10

// Глобальный массив
int shared_array[ARRAY_SIZE];

// Блокировка чтения-записи
pthread_rwlock_t rwlock;

// Условная переменная и мьютекс
pthread_cond_t condvar;
pthread_mutex_t cond_mutex;

// Флаг, указывающий, что данные обновлены
int data_updated = 0;

// Индекс записи для пишущего потока
int write_index = 0;

// Функция для пишущего потока
void* writer_thread(void* arg) {
    while (1) {
        // Захватываем блокировку на запись
        pthread_rwlock_wrlock(&rwlock);

        // Записываем текущее значение индекса в массив
        if (write_index < ARRAY_SIZE) {
            shared_array[write_index] = write_index + 1;
            printf("Writer wrote: %d\n", shared_array[write_index]);
            write_index++;
        }

        // Устанавливаем флаг обновления данных
        pthread_mutex_lock(&cond_mutex);
        data_updated = 1;
        pthread_cond_broadcast(&condvar);  // Сигнализируем читающим потокам
        pthread_mutex_unlock(&cond_mutex);

        // Освобождаем блокировку на запись
        pthread_rwlock_unlock(&rwlock);

        sleep(1);  // Ждем немного перед следующей записью
    }

    return NULL;
}

// Функция для читающих потоков
void* reader_thread(void* arg) {
    int tid = *((int*)arg);  // Получаем ID потока

    while (1) {
        // Ждем, пока данные не будут обновлены
        pthread_mutex_lock(&cond_mutex);
        while (!data_updated) {  // Проверяем флаг обновления
            pthread_cond_wait(&condvar, &cond_mutex);
        }
        pthread_mutex_unlock(&cond_mutex);

        // Захватываем блокировку на чтение
        pthread_rwlock_rdlock(&rwlock);

        // Выводим текущее состояние массива
        printf("Reader %d: Array state: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", shared_array[i]);
        }
        printf("\n");

        // Освобождаем блокировку на чтение
        pthread_rwlock_unlock(&rwlock);

        // Сбрасываем флаг обновления только один раз после чтения
        pthread_mutex_lock(&cond_mutex);
        data_updated = 0;
        pthread_mutex_unlock(&cond_mutex);
    }

    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[10];
    int reader_ids[10];

    // Инициализация блокировки чтения-записи, условной переменной и мьютекса
    pthread_rwlock_init(&rwlock, NULL);
    pthread_cond_init(&condvar, NULL);
    pthread_mutex_init(&cond_mutex, NULL);

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
    pthread_cond_destroy(&condvar);
    pthread_mutex_destroy(&cond_mutex);

    return 0;
}
