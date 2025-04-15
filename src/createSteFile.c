#include "../include/createSteFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>

#include "../include/SNLogger.h"
#include "../include/steFile.h"

#define BUFFER_SIZE 96

typedef struct _FILES {
    char **files;
    uint8_t size;
} FILES;

int getAllFileNames(FILES *files);

int createFile(char *argv);

int readTextFile(STE_FILE *steFile, const char *pszName);

char *allocateMemoryForFolderName(char *name, int iNameLength, int iFolderNameLength, char *folderName,
                                  const char *pszName);

void writeToFile(const STE_FILE *steFile, FILE *file);

int createAllFiles() {
    int iStatus = 0;
    FILES files;

    iStatus = getAllFileNames(&files);

    for (int i = 0; i < files.size; i++) {
    }

    if (iStatus != 0) {
        return iStatus;
    }

    for (int i = 0; i < files.size; i++) {
        iStatus = createFile(files.files[i]);
        if (iStatus != 0) {
            return iStatus;
        }
    }

    for (int i = 0; i < files.size; i++) {
        free(files.files[i]);
    }

    free(files.files);

    return iStatus;
}

int createFile(char *argv) {
    int8_t iStatus = 0;
    char *pszName = argv;
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
    FILE *file = NULL;
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
            if (name == NULL) {
                iStatus = 1;
                logError("Failed to allocate memory");
                free(steFile.body);
                steFile.body = NULL;
                return iStatus;
            }
            for (int i = rawImageFolderLength; i < originalFolderNameLength; i++) {
                name[i - rawImageFolderLength] = pszName[i];
            }
            name[originalFolderNameLength - rawImageFolderLength] = '\0';
        } else {
            name = calloc(originalFolderNameLength + 1, sizeof(char));
            if (name == NULL) {
                iStatus = 1;
                logError("Failed to allocate memory");
                free(steFile.body);
                steFile.body = NULL;
                return iStatus;
            }
            strncpy(name, pszName, originalFolderNameLength);
            name[originalFolderNameLength] = '\0';
        }
    } else {
        name = calloc(originalFolderNameLength + 1, sizeof(char));
        if (name == NULL) {
            iStatus = 1;
            logError("Failed to allocate memory");
            free(steFile.body);
            steFile.body = NULL;
            return iStatus;
        }
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

    if (iStatus == 0) {
        file = fopen(pszFileName, "w");
        if (file == NULL) {
            iStatus = 1;
            printf("Couldn't open file");
        } else {
            writeToFile(&steFile, file);
        }
        printf("Successfully made \"%s\" out of %s\n", pszFileName, name);
        logInfo("Successfully made \"%s\" out of %s", pszFileName, name);
    }

    free(pszFileName);
    pszFileName = NULL;
    free(name);
    name = NULL;
    free(steFile.body);
    steFile.body = NULL;

    return iStatus;
}

int getAllFileNames(FILES *files) {
    struct dirent *dirent;
    DIR *dir;
    int iStatus = 0;

    char *directoryName = "rawImageFiles";

    if ((dir = opendir(directoryName)) == NULL) {
        iStatus = 1;
        logError("Could not open the directory %s", directoryName);
        return iStatus;
    }

    char **fileNames = NULL;
    int count = 0;

    while ((dirent = readdir(dir)) != NULL) {
        int lengthOfFileName = strlen(dirent->d_name);
        if (strncmp(&dirent->d_name[lengthOfFileName-4], ".txt", 4) == 0) {
            count++;
        }
    }

    if (count == 0) {
        iStatus = 1;
        printf("No files in the folder");
    } else {
        fileNames = calloc(count, sizeof(char *));
        if (fileNames == NULL) {
            iStatus = 1;
            logError("Failed to allocate memory");
        } else {
            files->size = count;
            count = 0;
            rewinddir(dir);
            while ((dirent = readdir(dir)) != NULL) {
                int lengthOfFileName = strlen(dirent->d_name);
                // +2 for / in %s/%s
                if (lengthOfFileName > 4) {
                    if (strncmp(&dirent->d_name[lengthOfFileName-4], ".txt", 4) == 0) {
                        fileNames[count] = calloc(lengthOfFileName + strlen(directoryName) + 2, sizeof(char));
                        if (fileNames[count] == NULL) {
                            iStatus = 1;
                        } else {
                            sprintf(fileNames[count], "%s/%s", directoryName, dirent->d_name);
                            fileNames[count][lengthOfFileName + strlen(directoryName) + 1] = '\0';
                        }
                    }
                }
                if (iStatus != 0) {
                    for (int i = 0; i < count; i++) {
                        free(fileNames[i]);
                    }
                    free(fileNames);
                    return iStatus;
                }
                count++;
            }
        }
    }

    closedir(dir);

    files->files = fileNames;

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

    FILE *file = NULL;

    char buffer[BUFFER_SIZE] = {0};
    char *folderName = "rawImageFiles/";
    int iNameLength = strlen(pszName);

    int iFolderNameLength = strlen(folderName);

    char *name = NULL;

    logInfo("Reading %s", pszName);

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
                return iStatus;
            }
            strncpy(name, pszName, iNameLength);
            name[iNameLength] = '\0';
        }
    } else {
        name = allocateMemoryForFolderName(name, iNameLength, iFolderNameLength, folderName, pszName);
        if (name == NULL) {
            iStatus = 1;
            return iStatus;
        }
    }

    uint8_t *bodyBuffer = NULL;

    if (access(name, F_OK) != F_OK) {
        iStatus = 1;
        logError("File \"%s\" doesnt exist", name);
    } else {
        file = fopen(name, "r");
        if (file == NULL) {
            iStatus = 1;
            logError("Failed to open file: %s", name);
        } else {
            if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
                logError("Buffer is null, failed fgets in file %s", pszName);
                iStatus = 1;
            }
            width = strtok(buffer, delimiter);
            height = strtok(NULL, delimiter);
            iHeightLength = strlen(height);

            height[iHeightLength - 1] = '\0';

            iWidth = strtol(width, &endpointer, 10);
            if (endpointer == width) {
                iStatus = 1;
                logError("No digits were found");
                printf("No digits were found.\n");
            } else {
                if (*endpointer != '\0') {
                    iStatus = 1;
                    logError("Invalid character: %c\n", *endpointer);
                    printf("Invalid character: %c\n", *endpointer);
                } else {
                    logInfo("%s width: %ld", name, iWidth);

                    iHeight = strtol(height, &endpointer, 10);
                    if (endpointer == width) {
                        iStatus = 1;
                        printf("No digits were found.\n");
                    } else {
                        if (*endpointer != '\0') {
                            iStatus = 1;
                            printf("Invalid character: %c\n", *endpointer);
                        } else {
                            logInfo("%s height: %ld", name, iHeight);

                            steFile->width = iWidth;
                            steFile->height = iHeight;

                            steFile->body = calloc((iWidth * iHeight), sizeof(uint8_t));
                            bodyBuffer = calloc(iWidth + 1, sizeof(uint8_t));

                            if (bodyBuffer == NULL) {
                                iStatus = 1;
                                logError("Failed to allocate memory");
                                return iStatus;
                            } else {
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
                                free(bodyBuffer);
                                bodyBuffer = NULL;
                            }
                        }
                    }
                }
            }
        }
        fclose(file);
    }

    free(name);
    name = NULL;

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
    fclose(file);
}
