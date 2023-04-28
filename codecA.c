#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
    This code defines two functions encode_codecA and decode_codecA
    that perform encoding and decoding operations on a given string.
*/

/*
    The encode_codecA function takes a null-terminated string as input, and iterates through each character in the string.
    If the character is lowercase, it is converted to uppercase; if the character is uppercase, it is converted to lowercase.
    The function then returns the modified string.
*/
char *encode_codecA(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (islower(str[i]))
        {
            str[i] = toupper(str[i]);
        }
        else if (isupper(str[i]))
        {
            str[i] = tolower(str[i]);
        }
    }

    return str;
}

/*
    The decode_codecA function simply calls the encode_codecA function and returns its result.
    This indicates that the decoding operation is the same as the encoding operation, which is a characteristic of symmetric encryption algorithms.
*/
char *decode_codecA(char *str)
{
    return encode_codecA(str); // The decode function is the same as the encode function
}
