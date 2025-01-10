
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define SHM_KEY 12345 // Ключ для разделяемой памяти
#define SHM_SIZE sizeof(time_t) + sizeof(pid_t) + 256 // Размер разделяемой памяти

// Структура для передачи данных (время, pid, строка)
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Проверка, не существует ли уже процесс с таким именем
    if (getppid() == 1) {
        printf("Error: sender process is already running.\n");
        exit(1);
    }

    // Получаем идентификатор разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
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

    // Заполнение структуры данными
    shm_ptr->pid = getpid();
    time(&(shm_ptr->current_time));
    strcpy(shm_ptr->message, "This is the message from sender.");

    // Бесконечный цикл передачи данных
    while (1) {
        // Обновляем текущее время
        time(&(shm_ptr->current_time));
        sleep(1); // Задержка 1 секунда
    }

    // Отключаем разделяемую память
    shmdt(shm_ptr);
    return 0;
}
