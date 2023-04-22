#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int cmp(char *file1, char *file2, int verbose, int ignore_case) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (f1 == NULL || f2 == NULL) {
        perror("Error opening file");
        return 1;
    }

    int ch1, ch2;
    int is_equal = 1;
    int line_num = 1;

    while ((ch1 = fgetc(f1)) != EOF && (ch2 = fgetc(f2)) != EOF) {
        if (ignore_case) {
            ch1 = tolower(ch1);
            ch2 = tolower(ch2);
        }

        if (ch1 != ch2) {
            is_equal = 0;

            if (verbose) {
                printf("Files %s and %s differ at line %d\n", file1, file2, line_num);
                break;
            } else {
                break;
            }
        }

        if (ch1 == '\n') {
            line_num++;
        }
    }

    if (is_equal && fgetc(f1) != fgetc(f2)) {
        is_equal = 0;

        if (verbose) {
            printf("Files %s and %s have different lengths\n", file1, file2);
        }
    }

    fclose(f1);
    fclose(f2);

    if (is_equal) {
        if (verbose) {
            printf("Files %s and %s are equal\n", file1, file2);
        }

        return 0;
    } else {
        if (verbose == 0) {
            return 1;
        } else {
            return 1;
        }
    }
}

int main(int argc, char *argv[]) {
    int verbose = 0;
    int ignore_case = 0;

    if (argc != 3 && argc != 4) {
        printf("Usage: cmp <file1> <file2> [-v] [-i]\n");
        return 1;
    }

    if (argc == 4) {
        if (strcmp(argv[3], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[3], "-i") == 0) {
            ignore_case = 1;
        } else {
            printf("Unknown option: %s\n", argv[3]);
            return 1;
        }
    }

    if (argc == 5) {
        if (strcmp(argv[4], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[4], "-i") == 0) {
            ignore_case = 1;
        } else {
            printf("Unknown option: %s\n", argv[4]);
            return 1;
        }
    }

    return cmp(argv[1], argv[2], verbose, ignore_case);
}
