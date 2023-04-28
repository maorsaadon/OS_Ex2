#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

/*
    This program encodes a message using a specified encoding algorithm, and prints the encoded message to the console.
    The program takes two command-line arguments: the name of the encoding algorithm to use ("codecA" or "codecB"), and the message to encode.
    If the program is not called with exactly two arguments, it will print an error message and return 1.
    If the specified encoding algorithm is not recognized (i.e., it is not "codecA" or "codecB"), the program will print an error message and return 1.
    Otherwise, the program will call the appropriate encoding function (either encode_codecA or encode_codecB) with the provided message as an argument. 
    The resulting encoded message will be stored in the outputvariable.
    Finally, the program will print the encoded message to the console and return 0.
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
        output = encode_codecA(argv[2]);
    }
    else if (strcmp(argv[1], "codecB") == 0)
    {
        output = encode_codecB(argv[2]);
    }
    else
    {
        printf("Invalid codec: %s\n", argv[1]);
        return 1;
    }

    printf("Encoded string: %s\n", output);
    return 0;
}
