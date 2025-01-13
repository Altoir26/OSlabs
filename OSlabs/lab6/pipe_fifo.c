#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_PATH "/tmp/my_fifo"

void get_current_time(char *buffer, size_t size) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void pipe_example() {
    int pipefd[2];
    pid_t pid;
    char buffer[128];
    char time_str[64];
    
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Родительский процесс
        close(pipefd[0]); // Закрываем конец для чтения
        get_current_time(time_str, sizeof(time_str));
        snprintf(buffer, sizeof(buffer), "Parent PID: %d, Time: %s", getpid(), time_str);
        write(pipefd[1], buffer, strlen(buffer) + 1);
        close(pipefd[1]); // Закрываем конец для записи
        sleep(5); // Ждем 5 секунд
    } else {
        // Дочерний процесс
        close(pipefd[1]); // Закрываем конец для записи
        read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]); // Закрываем конец для чтения
        
        get_current_time(time_str, sizeof(time_str));
        printf("Child Time: %s\nReceived: %s\n", time_str, buffer);
        exit(EXIT_SUCCESS);
    }
}

void fifo_example() {
    pid_t pid;
    char buffer[128];
    char time_str[64];

    // Создаем FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Родительский процесс
        int fifo_fd = open(FIFO_PATH, O_WRONLY);
        get_current_time(time_str, sizeof(time_str));
        snprintf(buffer, sizeof(buffer), "Parent PID: %d, Time: %s", getpid(), time_str);
        write(fifo_fd, buffer, strlen(buffer) + 1);
        close(fifo_fd);
        sleep(5); // Ждем 5 секунд
    } else {
        // Дочерний процесс
        int fifo_fd = open(FIFO_PATH, O_RDONLY);
        read(fifo_fd, buffer, sizeof(buffer));
        close(fifo_fd);
        
        get_current_time(time_str, sizeof(time_str));
        printf("Child Time: %s\nReceived: %s\n", time_str, buffer);
        unlink(FIFO_PATH); // Удаляем FIFO
        exit(EXIT_SUCCESS);
    }
}

int main() {
    printf("Pipe Example:\n");
    pipe_example();

    printf("\nFIFO Example:\n");
    fifo_example();

    return 0;
}