// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define SHM_KEY 12345 // Ключ для разделяемой памяти
#define SEM_KEY 54321 // Ключ для семафора
#define SHM_SIZE 4096 // Размер разделяемой памяти

// Определение структуры shared_data
struct shared_data {
    time_t current_time;   // Время
    pid_t pid;             // PID процесса
    char message[256];     // Сообщение
};

// Функция для блокировки семафора
void lock_semaphore(int semid) {
    struct sembuf op = {0, -1, 0}; // Уменьшаем семафор на 1
    if (semop(semid, &op, 1) == -1) {
        perror("semop lock failed");
        exit(1);
    }
}

// Функция для разблокировки семафора
void unlock_semaphore(int semid) {
    struct sembuf op = {0, 1, 0}; // Увеличиваем семафор на 1
    if (semop(semid, &op, 1) == -1) {
        perror("semop unlock failed");
        exit(1);
    }
}

int main() {
    // Подключаемся к разделяемой памяти
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Подключаем разделяемую память
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Подключаемся к семафору
    int semid = semget(SEM_KEY, 1, 0666);
    if (semid == -1) {
        perror("semget failed");
        shmdt(data); // Отключаем память при ошибке
        exit(1);
    }

    // Бесконечный цикл приема данных
    while (1) {
        // Захватываем семафор
        lock_semaphore(semid);

        // Читаем данные из разделяемой памяти
        char time_str[100];
        struct tm *tm_info = localtime(&(data->current_time));
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        // Выводим данные на экран
        printf("Receiver PID: %d, Current Time: %s, Sender PID: %d, Message: %s\n",
               getpid(), time_str, data->pid, data->message);

        // Освобождаем семафор
        unlock_semaphore(semid);

        sleep(1); // Задержка 1 секунда
    }

    // Отключаем разделяемую память
    shmdt(data);

    return 0;
}
