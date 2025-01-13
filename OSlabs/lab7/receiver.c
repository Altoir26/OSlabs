// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>     // Для O_RDWR
#include <sys/mman.h>  // Для mmap, munmap, shm_open
#include <sys/types.h>

#define SHM_NAME "/my_shared_memory" // Имя разделяемой памяти
#define SHM_SIZE 4096                // Размер разделяемой памяти (4 KB)

// Структура для получения данных
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Подключаемся к существующей разделяемой памяти
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error: shared memory does not exist or cannot be accessed");
        exit(1);
    }

    // Подключаем разделяемую память
    void *shm_ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    struct shared_data *data = (struct shared_data *)shm_ptr;

    // Бесконечный цикл приема данных
    while (1) {
        // Читаем данные из разделяемой памяти
        char time_str[100];
        struct tm *tm_info = localtime(&(data->current_time));
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        // Выводим данные на экран
        printf("Receiver PID: %d, Current Time: %s, Sender PID: %d, Message: %s\n",
               getpid(), time_str, data->pid, data->message);

        sleep(1); // Задержка 1 секунда
    }

    // Отключаем разделяемую память
    munmap(shm_ptr, SHM_SIZE);
    return 0;
}
