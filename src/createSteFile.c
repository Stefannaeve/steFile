#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/SNLogger.h"
#include "../include/steFile.h"

#define BUFFER_SIZE 96

int readTextFile(STE_FILE *steFile, const char *pszName);
char *allocateMemoryForFolderName(char *name, int iNameLength, int iFolderNameLength, char *folderName,
                                  const char *pszName);
void writeToFile(const STE_FILE *steFile, FILE *file);

int createFile(const char **argv) {
    int8_t iStatus = 0;
    const char *pszName = argv[2];
    const unsigned int iHeightLength;
    const unsigned int iWidthLength;
    unsigned int iLengthOfName = 0;
    unsigned int checkIfExtensionValue = 1;
    char *pszFileName;
    char *imageFolder = "images/";
    char *rawImageFolderName = "rawImageFiles/";
    int rawImageFolderLength = strlen(rawImageFolderName);
    int imageFolderLength = strlen(imageFolder);
    char szCheckIfExtension[5];
    FILE *file;
    STE_FILE steFile;

    int originalFolderNameLength = strlen(pszName);

    char *name = NULL;

    memcpy(steFile.magicValue, MAGIC_VALUE, 4);
    steFile.magicValue[4] = '\0';

    iStatus = readTextFile(&steFile, pszName);

    if (iStatus != 0) {
        return iStatus;
    }


    // Check if the prefix "rawImageFiles/" is here, if it is, remove it
    if (rawImageFolderLength < originalFolderNameLength) {
        if (strncmp(pszName, rawImageFolderName, rawImageFolderLength) == 0) {
            name = calloc(originalFolderNameLength - rawImageFolderLength + 1, sizeof(char));
            for (int i = rawImageFolderLength; i < originalFolderNameLength; i++) {
                name[i-rawImageFolderLength] = pszName[i];
            }
            name[originalFolderNameLength - rawImageFolderLength] = '\0';
        } else {
            name = calloc(originalFolderNameLength + 1, sizeof(char));
            strncpy(name, pszName, originalFolderNameLength);
            name[originalFolderNameLength] = '\0';
        }
    } else {
        name = calloc(originalFolderNameLength + 1, sizeof(char));
        strncpy(name, pszName, originalFolderNameLength);
        name[originalFolderNameLength] = '\0';
    }

    iLengthOfName = strlen(name);

    if (iLengthOfName > LENGTH_OF_EXTENSION) {
        for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
            szCheckIfExtension[i] = name[iLengthOfName - LENGTH_OF_EXTENSION + i];
        }
        szCheckIfExtension[LENGTH_OF_EXTENSION] = '\0';

        checkIfExtensionValue = strncmp(szCheckIfExtension, FILE_EXTENSION, 4);
    }

    if (checkIfExtensionValue != 0) {
        pszFileName = (char *) calloc(iLengthOfName - 4 + imageFolderLength + LENGTH_OF_EXTENSION + 1,
                                      sizeof(char));

        if (pszFileName == NULL) {
            iStatus = 1;
            printf("Memory allocation issue");
        } else {
            strncat(pszFileName, imageFolder, imageFolderLength);
            strncat(pszFileName, name, iLengthOfName - 4);
            strncat(pszFileName, FILE_EXTENSION, LENGTH_OF_EXTENSION + 1);
            pszFileName[iLengthOfName - 4 + LENGTH_OF_EXTENSION + imageFolderLength] = '\0';
        }
    } else {
        pszFileName = (char *) calloc(iLengthOfName + imageFolderLength + 1,
                                      sizeof(char));

        if (pszFileName == NULL) {
            iStatus = 1;
            printf("Memory allocation issue");
        } else {
            strncat(pszFileName, imageFolder, imageFolderLength);
            strncat(pszFileName, pszName, iLengthOfName);
            pszFileName[iLengthOfName + imageFolderLength] = '\0';
        }
    }

    logDebug("Images Folder Name: %s", pszFileName);

    file = fopen(pszFileName, "w");
    if (file == NULL) {
        iStatus = 1;
        printf("Couldn't open file");
    } else {
        writeToFile(&steFile, file);
    }
    free(pszFileName);
    free(name);


    return iStatus;
}

int readTextFile(STE_FILE *steFile, const char *pszName) {
    int iStatus = 0;
    const char *delimiter = " ";
    char *width;
    char *height;
    long int iWidth = 0;
    long int iHeight = 0;
    int iHeightLength = 0;
    char *endpointer;

    char buffer[BUFFER_SIZE] = {0};
    char *folderName = "rawImageFiles/";
    int iNameLength = strlen(pszName);

    int iFolderNameLength = strlen(folderName);

    char *name = NULL;

    if (iNameLength > iFolderNameLength) {
        if (!strncmp(pszName, folderName, iNameLength)) {
            name = allocateMemoryForFolderName(name, iNameLength, iFolderNameLength, folderName, pszName);
            if (name == NULL) {
                iStatus = 1;
                return iStatus;
            }
        } else {
            name = calloc(iNameLength + 1, sizeof(
                              char));
            if (name == NULL) {
                iStatus = 1;
                logError("Failed to allocate memory");
                printf("Failed to allocate memory\n");
            } else {
                strncpy(name, pszName, iNameLength);
                name[iNameLength] = '\0';
            }
        }
    } else {
        name = allocateMemoryForFolderName(name, iNameLength, iFolderNameLength, folderName, pszName);
        if (name == NULL) {
            iStatus = 1;
            return iStatus;
        }
    }

    uint8_t *bodyBuffer;

    if (access(name, F_OK) != F_OK) {
        iStatus = 1;
        printf("File \"%s\" doesnt exist\n", name);
    } else {
        FILE *file = fopen(name, "r");
        if (file == NULL) {
            iStatus = 1;
            logError("Failed to open file: %s", name);
            printf("Failed to open file: %s", name);
        } else {
            if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
                logError("Buffer is null, failed fgets\n");
                iStatus = 1;
            }
            width = strtok(buffer, delimiter);
            printf("%s", width);
            height = strtok(NULL, delimiter);
            printf(":%s\n", height);
            iHeightLength = strlen(height);
            printf("Length of height: %d\n", iHeightLength);

            height[iHeightLength - 1] = '\0';

            iWidth = strtol(width, &endpointer, 10);
            if (endpointer == width) {
                iStatus = 1;
                printf("No digits were found.\n");
                return iStatus;
            }
            if (*endpointer != '\0') {
                iStatus = 1;
                printf("Invalid character: %c\n", *endpointer);
                return iStatus;
            }

            printf("The width is: %ld\n", iWidth);

            iHeight = strtol(height, &endpointer, 10);
            if (endpointer == width) {
                iStatus = 1;
                printf("No digits were found.\n");
                return iStatus;
            }
            if (*endpointer != '\0') {
                iStatus = 1;
                printf("Invalid character: %c\n", *endpointer);
                return iStatus;
            }

            printf("The height is: %ld\n", iHeight);

            steFile->width = iWidth;
            steFile->height = iHeight;

            steFile->body = calloc((iWidth * iHeight), sizeof(uint8_t));
            bodyBuffer = calloc(iWidth + 1, sizeof(uint8_t));

            uint8_t character = 0;

            for (int i = 0; i < iHeight; i++) {
                memset(bodyBuffer, 0, iWidth + 1);
                for (int j = 0; j < iWidth; j++) {
                    character = getc(file);
                    if (character == '1') {
                        character = 0xFF;
                    } else {
                        character -= 48;
                    }
                    bodyBuffer[j] = character;
                }
                for (int j = 0; j < iWidth; j++) {
                    steFile->body[i * iWidth + j] = bodyBuffer[j];
                }
                while ((character = getc(file)) != '\n') {
                    if (character == EOF || character == 255) {
                        break;
                    }
                }
            }
        }
    }
    free(name);


    return iStatus;
}

char *allocateMemoryForFolderName(char *name, int iNameLength, int iFolderNameLength, char *folderName,
                                  const char *pszName) {
    name = calloc(iNameLength + iFolderNameLength + 1, sizeof(
                      char));
    if (name == NULL) {
        logError("Failed to allocate memory");
        printf("Failed to allocate memory\n");
    } else {
        strncat(name, folderName, iFolderNameLength);
        strncat(name, pszName, iNameLength);
        name[iNameLength + iFolderNameLength] = '\0';
    }
    return name;
}

void writeToFile(const STE_FILE *steFile, FILE *file) {
    fputs(steFile->magicValue, file);
    fputc(steFile->width, file);
    fputc(steFile->height, file);
    fwrite(steFile->body, 1, steFile->width * steFile->height, file);
    //fputs(steFile->body, file);
    fclose(file);
}