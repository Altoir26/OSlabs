#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: mygrep pattern [file ...]\n");
}

void search_pattern(FILE *fp, const char *pattern) {
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, pattern)) {
            printf("%s", line);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return EXIT_FAILURE;
    }

    const char *pattern = argv[1];
    int file_index = 2;

    // Process files or standard input
    if (file_index == argc) {
        search_pattern(stdin, pattern);
    } else {
        for (; file_index < argc; file_index++) {
            FILE *fp = fopen(argv[file_index], "r");
            if (!fp) {
                perror(argv[file_index]);
                continue;
            }
            search_pattern(fp, pattern);
            fclose(fp);
        }
    }

    return EXIT_SUCCESS;
}
