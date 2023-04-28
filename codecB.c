#include <stdlib.h>
#include <string.h>

/*
    This C code contains two functions, encode_codecB and decode_codecB,
    which are used for encoding and decoding a string using Codec B.
*/

/*
    The encode_codecB function takes a pointer to a string as input and modifies the string in-place by adding 3 to
    the ASCII value of each character in the string and wrapping around if necessary
    (i.e., if the resulting value is greater than 255, it will wrap around to 0 and start incrementing from there).
    The function then returns the modified string.
*/
char *encode_codecB(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        str[i] = (str[i] + 3) % 256; // Adding 3 to the ASCII value, wrapping around if necessary
    }

    return str;
}

/*
    The decode_codecB function also takes a pointer to a string as input and modifies the string in-place by
    subtracting 3 from the ASCII value of each character in the string and wrapping around if necessary.
    The function then returns the modified string.
*/
char *decode_codecB(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        str[i] = (str[i] - 3 + 256) % 256; // Subtracting 3 from the ASCII value, wrapping around if necessary
    }

    return str;
}
