#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 10

// Глобальный массив
int shared_array[ARRAY_SIZE];

// Мьютекс и условная переменная
pthread_mutex_t mutex;
pthread_cond_t condvar;

// Индекс записи для пишущего потока
int write_index = 0;

// Функция для пишущего потока
void* writer_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);  // Захват мьютекса

        // Записываем текущее значение индекса в массив
        if (write_index < ARRAY_SIZE) {
            shared_array[write_index] = write_index + 1;
            printf("Writer wrote: %d\n", shared_array[write_index]);
            write_index++;
        }

        // Сигнализируем читающим потокам, что данные изменились
        pthread_cond_broadcast(&condvar);

        pthread_mutex_unlock(&mutex);  // Освобождение мьютекса

        sleep(1);  // Ждем немного перед следующим записыванием
    }

    return NULL;
}

// Функция для читающих потоков
void* reader_thread(void* arg) {
    int tid = *((int*)arg);  // Получаем ID потока

    while (1) {
        pthread_mutex_lock(&mutex);  // Захват мьютекса

        // Ждем, пока пишущий поток что-то запишет
        pthread_cond_wait(&condvar, &mutex);

        // Выводим текущее состояние массива
        printf("Reader %d: Array state: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", shared_array[i]);
        }
        printf("\n");

        pthread_mutex_unlock(&mutex);  // Освобождение мьютекса

        sleep(1);  // Ждем немного перед следующим чтением
    }

    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[10];
    int reader_ids[10];

    // Инициализация мьютекса и условной переменной
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);

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
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condvar);

    return 0;
}
