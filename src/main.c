#include <stdio.h>
#include <string.h>

#include "../include/SNLogger.h"
#include "../include/main.h"
#include "../include/windowApplicationSteFile.h"

#define FIRST_ARGUMENT_LENGTH 2
#define CREA "-c"
#define READ "-r"

// CREA
// READ

int main(int argc, const char *argv[]) {
    int iFirstArgLength = 0;
    int iStatus = 0;

    if (argc < 2) {
        printf("You have to have more arguments\n");
        printf("Expected formats:\n");
        printf("Expected format one: ./main %s myfile(.txt)\n", CREA);
        printf("Expected format two: ./main %s\n", READ);
        return 1;
    }

    iFirstArgLength = strnlen(argv[1], FIRST_ARGUMENT_LENGTH + 1);

    if (iFirstArgLength != FIRST_ARGUMENT_LENGTH) {
        printf("Wrong length on first argument\n");
        printf("Expected formats:\n");
        printf("Expected format one: %s\n", CREA);
        printf("Expected format two: %s\n", READ);
        return 1;
    }

    if (strncmp(argv[1], CREA, FIRST_ARGUMENT_LENGTH) == 0) {
        if (argc < 3) {
            printf("Wrong arguments\n");
            printf("Expected format: ./main %s myfile(.txt)\n", CREA);
            return 1;
        }
        iStatus = createFile(argv);
    } else if (strncmp(argv[1], READ, FIRST_ARGUMENT_LENGTH) == 0) {
        iStatus = readSteFile();
    } else {
        iStatus = 1;
        printf("First argument is not in expected format, you have to write \"%s\" for create, or \"%s\" for read\n",
               CREA, READ);
    }

    return iStatus;
}
