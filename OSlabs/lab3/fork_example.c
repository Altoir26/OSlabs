#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

// Обработчик для сигнала SIGINT
void handle_sigint(int sig) {
    printf("Received signal: %d (%s)\n", sig, strsignal(sig));
}

// Обработчик для сигнала SIGTERM
void handle_sigterm(int sig, siginfo_t *info, void *context) {
    printf("Received signal: %d (%s)\n", sig, strsignal(sig));
    exit(0);
}

// Обработчик выхода
void on_exit_handler(void) {
    printf("Program terminated (atexit handler).\n");
}

int main() {
    // Установка обработчика выхода
    if (atexit(on_exit_handler) != 0) {
        perror("Error registering exit handler");
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGINT
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Error setting up SIGINT handler");
        return EXIT_FAILURE;
    }

    // Установка обработчика SIGTERM через sigaction
    struct sigaction sa;
    sa.sa_sigaction = handle_sigterm;
    sa.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("Error setting up SIGTERM handler");
        return EXIT_FAILURE;
    }

    pid_t pid = fork(); // Создание дочернего процесса
    if (pid < 0) {
        // Ошибка fork()
        perror("Error during fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Child process started. PID: %d\n", getpid());
        sleep(5); // Имитация работы дочернего процесса
        printf("Child process completed.\n");
        exit(42); // Завершаем процесс с кодом 42
    } else {
        // Родительский процесс
        int status;
        printf("Parent process is waiting for the child process to finish. PID: %d\n", getpid());
        wait(&status); // Ожидание завершения дочернего процесса

        // Проверка причины завершения дочернего процесса
        if (WIFEXITED(status)) {
            printf("Child process exited with code: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process terminated by signal: %d (%s)\n",
                   WTERMSIG(status), strsignal(WTERMSIG(status)));
        }
    }

    return EXIT_SUCCESS;
}
