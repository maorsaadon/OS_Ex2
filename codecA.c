#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char *encode_codecA(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (islower(str[i])) {
            str[i] = toupper(str[i]);
        } else if (isupper(str[i])) {
            str[i] = tolower(str[i]);
        }
    }

    return str;
}

char *decode_codecA(char* str) {
    return encode_codecA(str); // The decode function is the same as the encode function
}
