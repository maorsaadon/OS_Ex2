#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 1024

/*
    This program copies the contents of one file to another file.
    It provides an option to overwrite the destination file if it already exists or to abort the operation.
    The program can also display a message indicating whether the copy was successful or not.
*/

/*
    The copy_file function takes the name of the source and destination files,
    and a flag indicating whether to overwrite the destination file or not.
    It checks if the source file exists and opens it in binary read mode.
    Then, it checks if the destination file exists and opens it in binary write mode.
    If the destination file exists and the overwrite flag is not set, the function returns with an error message.
    Otherwise, the function reads from the source file in blocks of 1024 bytes and writes them to the destination file until the end of the file is reached.
    If any errors occur during the write operation, the function returns with an error message.
*/
int copy_file(char *name_of_file1, char *name_of_file2, int want_f_operation)
{

    FILE *f1;
    FILE *f2;

    // checking if source file is exists
    f1 = fopen(name_of_file1, "rb");
    if (f1 == NULL)
    {
        printf("There is no file to copy from!\n");
        return 1;
    }

    // checking if destantion file is already exists that will pr
    f2 = fopen(name_of_file2, "rb");
    if (f2 != NULL)
    {
        // file2 is exists
        if (want_f_operation != 1)
        {
            printf("target file exists\n");
            fclose(f1);
            fclose(f2);
            return 2;
        }
    }

    f2 = fopen(name_of_file2, "w");
    if (f2 == NULL)
    {
        printf("Cant create destination file\n");
        fclose(f1);
        return 1;
    }

    // copy to file 2 from file 1

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, 1024, f1)) > 0)
    {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, f2);
        if (bytes_written == -1)
        {
            printf("Error: cannot write to destination file\n");
            fclose(f2);
            fclose(f1);
            return 1;
        }
    }

    fclose(f1);
    fclose(f2);

    return 0;
}

/*
    In main, command-line arguments are parsed to extract the names of the source and destination files and any optional flags.
    If either file name is missing, an error message is displayed, and the program exits with a non-zero status code.
    If the -v flag is present, the copy_file function is called with the appropriate arguments, and a success or failure message is displayed based on the return value.
*/
int main(int argc, char **argv)
{

    char *name_of_file1 = NULL;
    char *name_of_file2 = NULL;

    int want_v_operation = 0;
    int want_f_operation = 0;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
        {
            want_v_operation = 1;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            want_f_operation = 1;
        }
        else if (name_of_file1 == NULL)
        {
            name_of_file1 = argv[i];
        }
        else if (name_of_file2 == NULL)
        {
            name_of_file2 = argv[i];
        }
        else
        {
            printf("Invaild Input! Need to be in this format : ./cmp <file1> <file2> [-v] [-i]\n");
            return -1;
        }
    }

    if (name_of_file1 == NULL || name_of_file2 == NULL)
    {
        printf("Error: You must specify two file names.\n");
        return 1;
    }

    else if (want_v_operation == 1)
    {
        int result = copy_file(name_of_file1, name_of_file2, want_f_operation);

        if (result == 0)
        {
            printf("Success\n");
        }
        else if (result == 1)
        {
            printf("general failure\n");
        }
    }

    return 0;
}
