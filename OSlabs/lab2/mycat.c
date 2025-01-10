#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void print_usage() {
    printf("Usage: mycat [-n] [-b] [-E] [file ...]\n");
}

void process_file(FILE *fp, bool number_lines, bool number_nonempty, bool show_ends) {
    char line[1024];
    int line_number = 1;

    while (fgets(line, sizeof(line), fp)) {
        if (number_lines) {
            printf("%6d\t", line_number++);
        } else if (number_nonempty && line[0] != '\n') {
            printf("%6d\t", line_number++);
        }

        printf("%s", line);
        if (show_ends && line[strlen(line) - 1] == '\n') {
            printf("$");
        }
    }
}

int main(int argc, char *argv[]) {
    bool number_lines = false, number_nonempty = false, show_ends = false;
    int file_index = 1;

    // Parse options
    for (; file_index < argc && argv[file_index][0] == '-'; file_index++) {
        char *opt = argv[file_index];
        for (int i = 1; opt[i]; i++) {
            if (opt[i] == 'n') {
                number_lines = true;
            } else if (opt[i] == 'b') {
                number_nonempty = true;
            } else if (opt[i] == 'E') {
                show_ends = true;
            } else {
                print_usage();
                return EXIT_FAILURE;
            }
        }
    }

    // Process files
    if (file_index == argc) {
        process_file(stdin, number_lines, number_nonempty, show_ends);
    } else {
        for (; file_index < argc; file_index++) {
            FILE *fp = fopen(argv[file_index], "r");
            if (!fp) {
                perror(argv[file_index]);
                continue;
            }
            process_file(fp, number_lines, number_nonempty, show_ends);
            fclose(fp);
        }
    }

    return EXIT_SUCCESS;
}
