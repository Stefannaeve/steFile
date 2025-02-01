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

void writeToFile(const STE_FILE *steFile, FILE *file);
int8_t widthAndHeightChecks(const unsigned int iHeightLength, const unsigned int iWidthLength, const char *pszHeight, const char *pszWidth);

int createFile(const char **argv) {
    int8_t iStatus = 0;
    const char *pszName = argv[2];
    const char *pszWidth = argv[3];
    const char *pszHeight = argv[4];
    const unsigned int iHeightLength = strlen(pszHeight);
    const unsigned int iWidthLength = strlen(pszWidth);
    unsigned int iLengthOfName = 0;
    unsigned int checkIfExtentionValue = 1;
    char *pszFileName;
    char szCheckIfExtension[5];
    FILE *file;
    STE_FILE steFile;

    iStatus = widthAndHeightChecks(iHeightLength, iWidthLength, pszHeight, pszWidth);

    if (iStatus != 0) {
        return 1;
    }

    steFile.height = (char *) calloc(iHeightLength + 1, sizeof(char));
    if (steFile.height == NULL) {
        iStatus = 1;
        printf("Memory allocation issue");
    } else {
        steFile.width = (char *) calloc(iWidthLength + 1, sizeof(char));
        if (steFile.width == NULL) {
            iStatus = 1;
            printf("Memory allocation issue");
        } else {
            memcpy(steFile.magicValue, MAGIC_VALUE, 4);
            steFile.magicValue[4] = '\0';

            memcpy(steFile.height, pszHeight, iHeightLength);
            memcpy(steFile.width, pszHeight, iWidthLength);

            iLengthOfName = strlen(pszName);

            if (iLengthOfName > LENGTH_OF_EXTENSION) {
                for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
                    szCheckIfExtension[i] = pszName[iLengthOfName - LENGTH_OF_EXTENSION + i];
                }
                szCheckIfExtension[LENGTH_OF_EXTENSION] = '\0';

                checkIfExtentionValue = strncmp(szCheckIfExtension, FILE_EXTENSION, 4);
            }

            if (checkIfExtentionValue == 0) {
                if (access(pszName, F_OK) == F_OK) {
                    iStatus = 1;
                    printf("File already exists\n");
                } else {
                    file = fopen(pszName, "w");
                    if (file == NULL) {
                        iStatus = 1;
                        printf("Couldnt open file");
                    } else {
                        writeToFile(&steFile, file);
                    }
                }
            } else {
                pszFileName = (char *) calloc(iLengthOfName + LENGTH_OF_EXTENSION + 1, sizeof(char));

                if (pszFileName == NULL) {
                    iStatus = 1;
                    printf("Memory allocation issue");
                } else {
                    strncat(pszFileName, pszName, iLengthOfName);
                    strcat(pszFileName, FILE_EXTENSION);
                    pszFileName[iLengthOfName + LENGTH_OF_EXTENSION] = '\0';

                    if (access(pszFileName, F_OK) == F_OK) {
                        iStatus = 1;
                        printf("File already exists\n");
                    } else {
                        file = fopen(pszFileName, "w");
                        if (file == NULL) {
                            iStatus = 1;
                            printf("Couldnt open file");
                        } else {
                            writeToFile(&steFile, file);
                        }
                    }
                    free(pszFileName);
                }
            }
            free(steFile.width);
        }
        free(steFile.height);
    }

    return iStatus;
}

void writeToFile(const STE_FILE *steFile, FILE *file) {
    fputs(steFile->magicValue, file);
    fputs(steFile->width, file);
    fputs(steFile->height, file);
    fputc('\n', file);
    fclose(file);
}

int8_t widthAndHeightChecks(const unsigned int iHeightLength, const unsigned int iWidthLength, const char *pszHeight, const char *pszWidth) {
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

    for (int i = 0; i < iHeightLength; i++) {
        if (pszHeight[i] < '0' || pszHeight[i] > '9') {
            printf("Height is in a invalid format, has to represent integers\n");
            return 1;
        }
    }

    if (pszWidth[0] == '0' || pszHeight[0] == '0') {
        printf("Width or height to small, should be between 4-99\n");
        return 1;
    }

    if (iHeightLength == 1 && pszHeight[0] < '4') {
        printf("Height is to small, should be between 4-99\n");
        return 1;
    }

    if (iWidthLength == 1 && pszWidth[0] < '4') {
        printf("Width is to small, should be between 4-99\n");
        return 1;
    }

    return 0;
}
