#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../include/SNLogger.h"
#include "../include/steFile.h"

#define FIRST_ARGUMENT_LENGTH 4
#define CREA "CREA\0"
#define READ "READ\0"

// CREA
// READ

int main(int argc, const char *argv[]) {
    int iFirstArgLength = 0;
    int iStatus = 0;

    logDebug("1");
    logError("1,5");

    if (argc < 2) {
        printf("You have to have more arguments\n");
        printf("Expected formats:\n");
        printf("Expected format one: ./main CREA myfile(.txt)\n");
        printf("Expected format two: ./main READ\n");
        return 1;
    }

    logDebug("2");

    iFirstArgLength = strnlen(argv[1], FIRST_ARGUMENT_LENGTH+1);

    if (iFirstArgLength != FIRST_ARGUMENT_LENGTH) {
        printf("Wrong length on first argument\n");
        printf("Expected formats:\n");
        printf("Expected format one: CREA\n");
        printf("Expected format two: READ\n");
        return 1;
    }

    if (strncmp(argv[1], CREA, 4) == 0){
        if (argc < 3) {
            printf("Wrong arguments\n");
            printf("Expected format: ./main CREA myfile(.txt)\n");
            return 1;
        }
        iStatus = createFile(argv);
    } else if (strncmp(argv[1], READ, 4) == 0) {
        iStatus = readSteFile();
    } else {
        iStatus = 1;
        printf("First argument is not in expected format, you have to write \"CREA\", or \"READ\"\n");
    }
    return iStatus;
}