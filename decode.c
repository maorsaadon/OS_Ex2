#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <codec> <message>\n", argv[0]);
        return 1;
    }

    char *output;
    
    if (strcmp(argv[1], "codecA") == 0) {
       output = decode_codecA(argv[2]);
    } else if (strcmp(argv[1], "codecB") == 0) {
        output = decode_codecB(argv[2]);
    } else {
        printf("Invalid codec: %s\n", argv[1]);
        return 1;
    }

    printf("Decoded string: %s\n", output);
    return 0;
}