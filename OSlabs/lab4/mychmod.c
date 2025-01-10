#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

// Функция для изменения прав с помощью символьного синтаксиса
void modify_permissions(const char *file, const char *mode) {
    struct stat st;
    
    // Получаем текущие права файла
    if (stat(file, &st) != 0) {
        perror("Ошибка при получении информации о файле");
        return;
    }
    
    // Переменная для хранения новых прав
    mode_t new_mode = st.st_mode;
    
    // Параметры для пользователей (u - user, g - group, o - others)
    char who = ' ';
    char op = ' ';
    char perm = ' ';

    // Обработка символов из строки mode
    for (int i = 0; i < strlen(mode); i++) {
        if (mode[i] == '+' || mode[i] == '-') {
            op = mode[i];
        } else if (mode[i] == 'u' || mode[i] == 'g' || mode[i] == 'o') {
            who = mode[i];
        } else if (mode[i] == 'r' || mode[i] == 'w' || mode[i] == 'x') {
            perm = mode[i];
            // Применяем изменения прав в зависимости от символов
            if (who == 'u') {
                if (op == '+') {
                    if (perm == 'r') new_mode |= S_IRUSR;
                    if (perm == 'w') new_mode |= S_IWUSR;
                    if (perm == 'x') new_mode |= S_IXUSR;
                } else if (op == '-') {
                    if (perm == 'r') new_mode &= ~S_IRUSR;
                    if (perm == 'w') new_mode &= ~S_IWUSR;
                    if (perm == 'x') new_mode &= ~S_IXUSR;
                }
            } else if (who == 'g') {
                if (op == '+') {
                    if (perm == 'r') new_mode |= S_IRGRP;
                    if (perm == 'w') new_mode |= S_IWGRP;
                    if (perm == 'x') new_mode |= S_IXGRP;
                } else if (op == '-') {
                    if (perm == 'r') new_mode &= ~S_IRGRP;
                    if (perm == 'w') new_mode &= ~S_IWGRP;
                    if (perm == 'x') new_mode &= ~S_IXGRP;
                }
            } else if (who == 'o') {
                if (op == '+') {
                    if (perm == 'r') new_mode |= S_IROTH;
                    if (perm == 'w') new_mode |= S_IWOTH;
                    if (perm == 'x') new_mode |= S_IXOTH;
                } else if (op == '-') {
                    if (perm == 'r') new_mode &= ~S_IROTH;
                    if (perm == 'w') new_mode &= ~S_IWOTH;
                    if (perm == 'x') new_mode &= ~S_IXOTH;
                }
            }
        }
    }

    // Устанавливаем новые права доступа
    if (chmod(file, new_mode) != 0) {
        perror("Ошибка при изменении прав доступа");
    } else {
        printf("Права доступа для файла %s изменены на: %o\n", file, new_mode & 0777);
    }
}

// Функция для изменения прав с помощью числового синтаксиса
void set_permissions_numeric(const char *file, const char *mode) {
    mode_t new_mode = strtol(mode, NULL, 8); // Преобразуем строку в числовое значение
    if (chmod(file, new_mode) != 0) {
        perror("Ошибка при изменении прав доступа");
    } else {
        printf("Права доступа для файла %s изменены на: %o\n", file, new_mode & 0777);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <режим> <файл>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    const char *file = argv[2];

    // Проверяем, если режим состоит из 3 цифр, то это числовой режим
    if (strlen(mode) == 3 && mode[0] >= '0' && mode[0] <= '7' && 
        mode[1] >= '0' && mode[1] <= '7' && mode[2] >= '0' && mode[2] <= '7') {
        set_permissions_numeric(file, mode);
    } else {
        modify_permissions(file, mode);
    }

    return EXIT_SUCCESS;
}
