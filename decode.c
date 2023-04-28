#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

/*
    This code is a C program that decodes a message using either "codecA" or "codecB", depending on the first command-line argument.
    The decoded string is printed to the console.
    The program expects two command-line arguments: the first is either "codecA" or "codecB", and the second is the message to decode.
    If there are not exactly two command-line arguments, an error message is displayed and the program exits with a return value of 1.
    If the first argument is "codecA", the message is decoded using the decode_codecA function defined in the "headers.h" header file.
    If the first argument is "codecB", the message is decoded using the decode_codecB function from the same header file.
    If the first argument is neither "codecA" nor "codecB", an error message is displayed and the program exits with a return value of 1.
    The decoded message is stored in the output variable, which is then printed to the console. The program exits with a return value of 0.
*/

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <codec> <message>\n", argv[0]);
        return 1;
    }

    char *output;

    if (strcmp(argv[1], "codecA") == 0)
    {
        output = decode_codecA(argv[2]);
    }
    else if (strcmp(argv[1], "codecB") == 0)
    {
        output = decode_codecB(argv[2]);
    }
    else
    {
        printf("Invalid codec: %s\n", argv[1]);
        return 1;
    }

    printf("Decoded string: %s\n", output);
    return 0;
}