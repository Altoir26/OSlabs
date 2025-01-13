// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>     // Для O_CREAT, O_RDWR
#include <sys/mman.h>  // Для mmap, munmap, shm_open
#include <sys/stat.h>  // Для S_IRUSR, S_IWUSR
#include <sys/types.h>

#define SHM_NAME "/my_shared_memory" // Имя разделяемой памяти
#define SHM_SIZE 4096                // Размер разделяемой памяти (4 KB)

// Структура для передачи данных
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Проверяем, существует ли уже разделяемая память
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error: shared memory already exists or cannot be created");
        exit(1);
    }

    // Устанавливаем размер разделяемой памяти
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    // Подключаем разделяемую память
    void *shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    struct shared_data *data = (struct shared_data *)shm_ptr;

    // Инициализация структуры
    data->pid = getpid();
    strcpy(data->message, "This is the message from sender.");

    // Бесконечный цикл передачи данных
    while (1) {
        // Обновляем текущее время
        time(&(data->current_time));
        sleep(1); // Задержка 1 секунда
    }

    // Отключаем разделяемую память
    munmap(shm_ptr, SHM_SIZE);
    shm_unlink(SHM_NAME);
    return 0;
}
