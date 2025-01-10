// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>  // Добавлен для определения флагов O_WRONLY, O_RDONLY

#define PIPE_NAME "/tmp/my_pipe"

// Структура для получения данных
struct shared_data {
    time_t current_time;
    pid_t pid;
    char message[256];
};

int main() {
    // Открытие канала для чтения
    int pipe_fd = open(PIPE_NAME, O_RDONLY);
    if (pipe_fd == -1) {
        perror("open failed");
        exit(1);
    }

    struct shared_data data;
    
    // Бесконечный цикл приема данных
    while (1) {
        // Чтение данных из канала
        if (read(pipe_fd, &data, sizeof(data)) == -1) {
            perror("read failed");
            close(pipe_fd);
            exit(1);
        }

        // Преобразуем время в строку
        char time_str[100];
        struct tm *tm_info = localtime(&(data.current_time));
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        // Вывод данных на экран
        printf("Receiver PID: %d, Current Time: %s, Message: %s\n", getpid(), time_str, data.message);

        sleep(1); // Задержка 1 секунда
    }

    // Закрытие канала
    close(pipe_fd);
    return 0;
}
