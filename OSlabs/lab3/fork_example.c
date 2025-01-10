#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// Обработчик для сигнала SIGINT
void handle_sigint(int sig) {
    printf("Получен сигнал: %d (%s)\n", sig, strsignal(sig));
}

// Обработчик для сигнала SIGTERM
void handle_sigterm(int sig, siginfo_t *info, void *context) {
    printf("Получен сигнал: %d (%s)\n", sig, strsignal(sig));
    exit(0);
}

// Обработчик выхода
void on_exit_handler(void) {
    printf("Программа завершена (обработчик atexit).\n");
}

int main() {
    // Установка обработчика выхода
    if (atexit(on_exit_handler) != 0) {
        perror("Ошибка установки обработчика выхода");
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Ошибка установки обработчика SIGINT");
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGTERM через sigaction
    struct sigaction sa;
    sa.sa_sigaction = handle_sigterm;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("Ошибка установки обработчика SIGTERM");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid < 0) {
        // Ошибка fork()
        perror("Ошибка вызова fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс запущен. PID: %d\n", getpid());
        pause(); // Ожидание сигналов
    } else {
        // Родительский процесс
        printf("Родительский процесс запущен. PID: %d\n", getpid());
        pause(); // Ожидание сигналов
    }

    return EXIT_SUCCESS;
}
