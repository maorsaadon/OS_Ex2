#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void print_usage(char *program_name) {
    printf("Usage: %s <source_file> <destination_file> [-v] [-f]\n", program_name);
}

int main(int argc, char *argv[]) {
    int opt, verbose = 0, force = 0;
    while ((opt = getopt(argc, argv, "vf")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'f':
                force = 1;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    if (opt + 2 > argc) {
        print_usage(argv[0]);
        return 1;
    }
    char *source_file = argv[opt];
    char *destination_file = argv[opt + 1];

    if (access(source_file, F_OK) == -1) {
        fprintf(stderr, "Error: %s does not exist\n", source_file);
        return 1;
    }

    if (!force && access(destination_file, F_OK) != -1) {
        fprintf(stderr, "Error: %s already exists\n", destination_file);
        return 1;
    }

    int source_fd = open(source_file, O_RDONLY);
    if (source_fd == -1) {
        fprintf(stderr, "Error: cannot open %s (%s)\n", source_file, strerror(errno));
        return 1;
    }

    int destination_fd = open(destination_file, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (destination_fd == -1) {
        fprintf(stderr, "Error: cannot create %s (%s)\n", destination_file, strerror(errno));
        close(source_fd);
        return 1;
    }

    char buffer[BUFSIZ];
    ssize_t bytes_read;
    while ((bytes_read = read(source_fd, buffer, BUFSIZ)) > 0) {
        ssize_t bytes_written = write(destination_fd, buffer, bytes_read);
        if (bytes_written == -1) {
            fprintf(stderr, "Error: cannot write to %s (%s)\n", destination_file, strerror(errno));
            close(source_fd);
            close(destination_fd);
            unlink(destination_file);
            return 1;
        }
    }

    close(source_fd);
    close(destination_fd);

    if (bytes_read == -1) {
        fprintf(stderr, "Error: cannot read from %s (%s)\n", source_file, strerror(errno));
        unlink(destination_file);
        return 1;
    }

    if (verbose) {
        printf("Success: %s copied to %s\n", source_file, destination_file);
    }

    return 0;
}
