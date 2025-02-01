#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../include/steFile.h"

#define FIRST_ARGUMENT_LENGTH 4
#define CREA "CREA\0"
#define READ "READ\0"

// CREA
// READ

int main(int argc, const char *argv[]) {
    int iFirstArgLength = 0;

    if (argc < 5) {
        printf("You have to have more arguments\n");
        return 1;
    }

    iFirstArgLength = strlen(argv[1]);

    if (iFirstArgLength != FIRST_ARGUMENT_LENGTH) {
        printf("Wrong length on first argument\n");
        return 1;
    }

    if (strncmp(argv[1], CREA, 4) == 0){
        createFile(argv);
        return 0;
    } else if (strncmp(argv[1], READ, 4) == 0) {
        printf("Jump to READ function\n");
    } else {
        printf("First argument is not in expected format, you have to write \"CREA\", or \"READ\"\n");
        return 1;
    }
    return 0;
}