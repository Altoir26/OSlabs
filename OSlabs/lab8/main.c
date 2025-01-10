#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Размер массива для записи
#define ARRAY_SIZE 10

// Общий массив для записи и чтения
char shared_array[ARRAY_SIZE];

// Мьютекс для синхронизации доступа к общему массиву
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Номер записи, который будет записываться в массив
int write_counter = 0;

// Функция для пишущего потока
void* writer_thread(void* arg) {
    while (1) {
        // Блокируем доступ другим потокам к общему ресурсу
        pthread_mutex_lock(&mutex);

        // Записываем текущий номер записи в общий массив
        if (write_counter < ARRAY_SIZE) {
            shared_array[write_counter] = 'A' + (write_counter % 26);  // Записываем символ
            printf("Writer wrote: %c at index %d\n", shared_array[write_counter], write_counter);
            write_counter++;
        }

        // Разблокируем мьютекс
        pthread_mutex_unlock(&mutex);

        // Пауза, чтобы дать шанс другим потокам
        usleep(100000);  // 100 ms
    }
    return NULL;
}

// Функция для читающих потоков
void* reader_thread(void* arg) {
    long tid = (long)arg;
    while (1) {
        // Блокируем доступ другим потокам к общему ресурсу
        pthread_mutex_lock(&mutex);

        // Выводим состояние общего массива
        printf("Reader %ld sees the array: ", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%c ", shared_array[i] ? shared_array[i] : '-');
        }
        printf("\n");

        // Разблокируем мьютекс
        pthread_mutex_unlock(&mutex);

        // Пауза, чтобы не перегружать вывод
        usleep(100000);  // 100 ms
    }
    return NULL;
}

int main() {
    pthread_t readers[10];  // Массив потоков для чтения
    pthread_t writer;       // Поток для записи

    // Создаем пишущий поток
    if (pthread_create(&writer, NULL, writer_thread, NULL)) {
        fprintf(stderr, "Error creating writer thread\n");
        return 1;
    }

    // Создаем читающие потоки
    for (long i = 0; i < 10; i++) {
        if (pthread_create(&readers[i], NULL, reader_thread, (void*)i)) {
            fprintf(stderr, "Error creating reader thread %ld\n", i);
            return 1;
        }
    }

    // Ожидаем завершения всех потоков
    pthread_join(writer, NULL);
    for (int i = 0; i < 10; i++) {
        pthread_join(readers[i], NULL);
    }

    return 0;
}
