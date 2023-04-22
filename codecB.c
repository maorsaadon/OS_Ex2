#include <stdlib.h>
#include <string.h>

char* encode_codecB(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        str[i] = (str[i] + 3) % 256; // Adding 3 to the ASCII value, wrapping around if necessary
    }

    return str;
}

char *decode_codecB(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        str[i] = (str[i] - 3 + 256) % 256; // Subtracting 3 from the ASCII value, wrapping around if necessary
    }

    return str;
}
