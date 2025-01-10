#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_NAME "my_fifo"

void get_current_time(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main() {
    pid_t pid;
    int pipe_fd[2];
    char parent_time[20];
    char message[100];

    // Создание pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создание FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {  // Родительский процесс
        // Получаем текущее время и PID
        get_current_time(parent_time, sizeof(parent_time));
        snprintf(message, sizeof(message), "Time: %s, PID: %d", parent_time, getpid());

        // Закрываем конец pipe, с которого родитель будет читать
        close(pipe_fd[0]);

        // Записываем сообщение в pipe
        write(pipe_fd[1], message, sizeof(message));
        close(pipe_fd[1]);

        // Задержка в 6 секунд для обеспечения разницы во времени
        sleep(6);

        // Чтение из FIFO
        int fifo_fd = open(FIFO_NAME, O_RDONLY);
        if (fifo_fd == -1) {
            perror("open FIFO for reading");
            exit(EXIT_FAILURE);
        }

        char fifo_message[100];  // Добавлена декларация переменной
        read(fifo_fd, fifo_message, sizeof(fifo_message));
        close(fifo_fd);

        printf("Parent received from FIFO: %s\n", fifo_message);

    } else {  // Дочерний процесс
        // Задержка на 5 секунд, чтобы время отличалось
        sleep(5);

        // Чтение из pipe
        close(pipe_fd[1]);  // Закрыть конец pipe, с которого дочерний процесс будет записывать
        char received_message[100];
        read(pipe_fd[0], received_message, sizeof(received_message));
        close(pipe_fd[0]);

        // Получаем текущее время дочернего процесса
        char child_time[20];
        get_current_time(child_time, sizeof(child_time));

        printf("Child received from pipe: %s\n", received_message);
        printf("Child current time: %s\n", child_time);

        // Запись в FIFO
        int fifo_fd = open(FIFO_NAME, O_WRONLY);
        if (fifo_fd == -1) {
            perror("open FIFO for writing");
            exit(EXIT_FAILURE);
        }

        char fifo_message[100];  // Добавлена декларация переменной
        snprintf(fifo_message, sizeof(fifo_message), "Child Time: %s, Child PID: %d", child_time, getpid());
        write(fifo_fd, fifo_message, sizeof(fifo_message));
        close(fifo_fd);
    }

    // Удаляем FIFO
    unlink(FIFO_NAME);

    return 0;
}
