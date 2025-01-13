// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define SHM_KEY 12345    // Ключ для разделяемой памяти
#define SEM_KEY 54321    // Ключ для семафора
#define SHM_SIZE 4096    // Размер разделяемой памяти

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
    // Создаем разделяемую память
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        perror("shmget failed or already exists");
        exit(1);
    }

    // Подключаем разделяемую память
    struct shared_data *data = (struct shared_data *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat failed");
        shmctl(shmid, IPC_RMID, NULL); // Удаляем память в случае ошибки
        exit(1);
    }

    // Создаем семафор
    int semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        perror("semget failed");
        shmctl(shmid, IPC_RMID, NULL); // Удаляем память в случае ошибки
        exit(1);
    }

    // Инициализируем семафор значением 1
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl initialization failed");
        semctl(semid, 0, IPC_RMID);    // Удаляем семафор в случае ошибки
        shmctl(shmid, IPC_RMID, NULL); // Удаляем память в случае ошибки
        exit(1);
    }

    // Заполняем разделяемую память
    data->pid = getpid();
    strcpy(data->message, "Message from sender");

    // Бесконечный цикл передачи данных
    while (1) {
        // Захватываем семафор
        lock_semaphore(semid);

        // Обновляем данные в разделяемой памяти
        time(&(data->current_time));

        // Освобождаем семафор
        unlock_semaphore(semid);

        sleep(1); // Задержка 1 секунда
    }

    // Деинициализация
    shmdt(data);                  // Отключаем разделяемую память
    shmctl(shmid, IPC_RMID, NULL); // Удаляем разделяемую память
    semctl(semid, 0, IPC_RMID);    // Удаляем семафор

    return 0;
}
