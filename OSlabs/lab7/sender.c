// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>  // Добавлен для определения флагов O_WRONLY, O_RDONLY

#define PIPE_NAME "/tmp/my_pipe"

// Структура для передачи данных
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Проверка на наличие файла канала
    if (access(PIPE_NAME, F_OK) != -1) {
        printf("Error: pipe already exists. Sender process can only be run once.\n");
        exit(1);
    }

    // Создание канала (FIFO)
    if (mkfifo(PIPE_NAME, 0666) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    // Открываем канал для записи
    int pipe_fd = open(PIPE_NAME, O_WRONLY);
    if (pipe_fd == -1) {
        perror("open failed");
        exit(1);
    }

    // Подготовка структуры данных для отправки
    struct shared_data data;
    data.pid = getpid();
    strcpy(data.message, "This is the message from sender.");

    // Бесконечный цикл передачи данных
    while (1) {
        // Получаем текущее время
        time(&(data.current_time));

        // Отправляем данные через канал
        if (write(pipe_fd, &data, sizeof(data)) == -1) {
            perror("write failed");
            close(pipe_fd);
            exit(1);
        }

        sleep(1); // Задержка 1 секунда
    }

    // Закрытие канала
    close(pipe_fd);
    return 0;
}
