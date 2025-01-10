
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define SHM_KEY 12345 // Ключ для разделяемой памяти
#define SHM_SIZE sizeof(time_t) + sizeof(pid_t) + 256 // Размер разделяемой памяти

// Структура для получения данных (время, pid, строка)
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Получаем идентификатор разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Подключаем разделяемую память
    struct shared_data *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Бесконечный цикл вывода данных
    while (1) {
        // Выводим время, pid и строку
        char time_str[100];
        struct tm *tm_info = localtime(&(shm_ptr->current_time));
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("Receiver PID: %d, Current Time: %s, Message: %s\n", getpid(), time_str, shm_ptr->message);
        sleep(1); // Задержка 1 секунда
    }

    // Отключаем разделяемую память
    shmdt(shm_ptr);
    return 0;
}
