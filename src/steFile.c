#include "../include/steFile.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAGIC_VALUE "STE0"
#define FILE_EXTENSION ".ste"
#define LENGTH_OF_EXTENSION 4


typedef struct _STE_FILE {
    char magicValue[5];
    char *width;
    char *height;

} STE_FILE;

int createFile(const char **argv) {
    const char *pszName = argv[2];
    const char *pszWidth = argv[3];
    const char *pszHeight = argv[4];
    const unsigned int iHeightLength = strlen(pszHeight);
    const unsigned int iWidthLength = strlen(pszWidth);
    unsigned int iLengthOfName = 0;
    char *pszFileName;
    char szCheckIfExtension[5];
    FILE *file;

    STE_FILE steFile;

    if (iWidthLength > 2 || iHeightLength > 2) {
        printf("Width or height to big, should be between 4-99\n");
        return 1;
    }

    for (int i = 0; i < iWidthLength; i++) {
        if (pszWidth[i] < '0' || pszWidth[i] > '9') {
            printf("Width is in a invalid format, has to represent integers\n");
            return 1;
        }
    }

    if (pszWidth[0] < '4' || pszHeight[0] < '4') {
        printf("Width or height to small, should be between 4-99\n");
        return 1;
    }

    for (int i = 0; i < iHeightLength; i++) {
        if (pszHeight[i] < '0' || pszHeight[i] > '9') {
            printf("Height is in a invalid format, has to represent integers\n");
            return 1;
        }
    }

    steFile.height = (char *) calloc(iHeightLength + 1, sizeof(char));
    steFile.width = (char *) calloc(iWidthLength + 1, sizeof(char));

    memcpy(steFile.magicValue, MAGIC_VALUE, 4);
    steFile.magicValue[4] = '\0';

    memcpy(steFile.height, pszHeight, iHeightLength);
    memcpy(steFile.width, pszHeight, iWidthLength);

    iLengthOfName = strlen(pszName);

    for (int i = 0; i < 4; i++) {
        szCheckIfExtension[i] = pszName[iLengthOfName-4+i];
    }
    szCheckIfExtension[4] = '\0';

    if (strncmp(szCheckIfExtension, FILE_EXTENSION, 4) == 0) {
        if (access(pszName, F_OK) == F_OK) {
            printf("File already exists\n");
            free(steFile.height);
            free(steFile.width);
            return 1;
        }
        file = fopen(pszName, "w");
    } else {
        pszFileName = (char *)calloc(iLengthOfName + LENGTH_OF_EXTENSION + 1, sizeof(char));

        strncat(pszFileName, pszName, iLengthOfName);
        strcat(pszFileName, FILE_EXTENSION);
        pszFileName[iLengthOfName + LENGTH_OF_EXTENSION] = '\0';

        if (access(pszFileName, F_OK) == F_OK) {
            printf("File already exists\n");
            free(steFile.height);
            free(steFile.width);
            free(pszFileName);
            return 1;
        }

        file = fopen(pszFileName, "w");

        free(pszFileName);
    }

    if (file == NULL) {
        printf("Error opening file\n");
        free(steFile.height);
        free(steFile.width);
        return 1;
    } else {
        printf("File created successfully\n");
    }

    fputs(steFile.magicValue, file);
    fputs(steFile.width, file);
    fputs(steFile.height, file);
    fputc('\n', file);

    free(steFile.height);
    free(steFile.width);


}
