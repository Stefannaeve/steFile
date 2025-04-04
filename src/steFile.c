#include "../include/steFile.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "../include/SNLogger.h"

#define MAGIC_VALUE "STE0"
#define FILE_EXTENSION ".ste"
#define LENGTH_OF_EXTENSION 4
#define SDL2_WIDTH 1024
#define SDL2_HEIGHT 1024
#define PIXEL_DENSITY 512
#define BLOCK_LENGTH 16
#define PIXEL_DENSITY2 (SDL2_WIDTH/BLOCK_LENGTH)
#define BUFFER_SIZE 96

enum COLOR {
    RED,
    GREEN,
    BLUE
};

enum COLOR_STRENGTH {
    ZERO,
    LOWER,
    LOW,
    MEDIUM,
    HIGH,
    HIGHER,
    MAX
};

typedef struct _PIXEL {
    uint8_t *pixels[BLOCK_LENGTH * BLOCK_LENGTH];
    uint8_t color;
} PIXEL;

typedef struct _STE_FILE {
    char magicValue[5];
    char width;
    char height;
    uint8_t *body;
} STE_FILE;

void makeColorForPixel(PIXEL *pixel, enum COLOR color, uint8_t colorValue);

void makeColor(enum COLOR color, uint8_t *value, uint8_t colorValue);

void makeColorRGB(uint8_t *value, uint8_t red, uint8_t green, uint8_t blue);

void writeToFile(const STE_FILE *steFile, FILE *file);

int readSteFile();

int8_t widthAndHeightChecks(unsigned int iHeightLength,
                            unsigned int iWidthLength,
                            const char *pszHeight,
                            const char *pszWidth);

int createFile2(const char **argv) {
    int8_t iStatus = 0;
    const char *pszName = argv[2];
    const char *pszWidth = argv[3];
    const char *pszHeight = argv[4];
    const unsigned int iHeightLength;
    const unsigned int iWidthLength;
    unsigned int iLengthOfName = 0;
    unsigned int checkIfExtensionValue = 1;
    char *pszFileName;
    char szCheckIfExtension[5];
    FILE *file;
    STE_FILE steFile;
    char *strtolRemainder = NULL;

    iStatus = widthAndHeightChecks(iHeightLength, iWidthLength, pszHeight, pszWidth);

    if (iStatus != 0) {
        return 1;
    }

    memcpy(steFile.magicValue, MAGIC_VALUE, 4);
    steFile.magicValue[4] = '\0';

    steFile.height = atoi(pszHeight);
    steFile.height = (uint8_t) strtol(pszHeight, &strtolRemainder, sizeof(pszHeight));
    steFile.width = atoi(pszWidth);

    iLengthOfName = strlen(pszName);

    if (iLengthOfName > LENGTH_OF_EXTENSION) {
        for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
            szCheckIfExtension[i] = pszName[iLengthOfName - LENGTH_OF_EXTENSION + i];
        }
        szCheckIfExtension[LENGTH_OF_EXTENSION] = '\0';

        checkIfExtensionValue = strncmp(szCheckIfExtension, FILE_EXTENSION, LENGTH_OF_EXTENSION);
    }

    if (checkIfExtensionValue == 0) {
        if (access(pszName, F_OK) == F_OK) {
            iStatus = 1;
            printf("File already exists\n");
        } else {
            file = fopen(pszName, "w");
            if (file == NULL) {
                iStatus = 1;
                printf("Couldn't open file");
            } else {
                writeToFile(&steFile, file);
            }
        }
    } else {
        pszFileName = (char *) calloc(iLengthOfName + LENGTH_OF_EXTENSION + 1,
                                      sizeof(char));

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
                    printf("Couldn't open file");
                } else {
                    writeToFile(&steFile, file);
                }
            }
            free(pszFileName);
        }
    }


    return iStatus;
}

int readTextFile(STE_FILE *steFile, const char *pszName) {
    int iStatus = 0;
    int position = 0;
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
    char *name = calloc(iNameLength + iFolderNameLength + 1, sizeof(
                            char));
    uint8_t *bodyBuffer;

    if (name == NULL) {
        iStatus = 1;
        logError("Failed to allocate memory");
        printf("Failed to allocate memory\n");
    } else {
        strncat(name, folderName, iFolderNameLength);
        strncat(name, pszName, iNameLength);

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
                // *endpointer = 0;

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
                        //logDebug("%d: character representation: %c", j, character);
                        if (character == '1') {
                            character = 0xFF;
                        } else {
                            character -= 48;
                        }
                        //logDebug("%d, character: %d", j, character);
                        bodyBuffer[j] = character;
                    }
                    //fgets(bodyBuffer, iWidth + 1, file);
                    //logDebug("buffer%d: %s", i, bodyBuffer);
                    //strncat(steFile->body, bodyBuffer, iWidth);
                    for (int j = 0; j < iWidth; j++) {
                        steFile->body[i * iWidth + j] = bodyBuffer[j];
                    }
                    //logDebug("Stefile%d: %s", i, steFile->body);
                    while ((character = getc(file)) != '\n') {
                        if (character == EOF || character == 255) {
                            break;
                        }
                    }
                }
            }
        }
        free(name);
    }
    return iStatus;
}

int createFile(const char **argv) {
    int8_t iStatus = 0;
    const char *pszName = argv[2];
    const unsigned int iHeightLength;
    const unsigned int iWidthLength;
    unsigned int iLengthOfName = 0;
    unsigned int checkIfExtensionValue = 1;
    char *pszFileName;
    char *imageFolder = "images/";
    int imageFolderLength = strlen(imageFolder);
    char szCheckIfExtension[5];
    FILE *file;
    STE_FILE steFile;

    logDebug("Hei");

    //iStatus = widthAndHeightChecks(iHeightLength, iWidthLength, pszHeight, pszWidth);

    /*
    if (iStatus != 0) {
        return 1;
    }
    */

    memcpy(steFile.magicValue, MAGIC_VALUE, 4);
    steFile.magicValue[4] = '\0';

    //steFile.height = atoi(pszHeight);
    //steFile.width = atoi(pszWidth);

    iStatus = readTextFile(&steFile, pszName);

    if (iStatus != 0) {
        return iStatus;
    }

    iLengthOfName = strlen(pszName);

    if (iLengthOfName > LENGTH_OF_EXTENSION) {
        for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
            szCheckIfExtension[i] = pszName[iLengthOfName - LENGTH_OF_EXTENSION + i];
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
            strncat(pszFileName, pszName, iLengthOfName - 4);
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

    file = fopen(pszFileName, "w");
    if (file == NULL) {
        iStatus = 1;
        printf("Couldn't open file");
    } else {
        writeToFile(&steFile, file);
    }
    free(pszFileName);


    return iStatus;
}

int readSteFile() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SDL2_WIDTH,
        SDL2_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB332,
        SDL_TEXTUREACCESS_STREAMING,
        SDL2_WIDTH, SDL2_HEIGHT);
    uint8_t rawPixels[SDL2_WIDTH * SDL2_HEIGHT];
    PIXEL *pixels = malloc(PIXEL_DENSITY2 * PIXEL_DENSITY2 * sizeof(PIXEL));

    int iYPlacement = 0;
    int iXPlacement = 0;
    int iYBlock = 0;
    int iXBlock = 0;
    int rawPixelPosition = 0;

    for (int y = 0; y < SDL2_WIDTH; y += BLOCK_LENGTH) {
        for (int x = 0; x < SDL2_WIDTH; x += BLOCK_LENGTH) {
            iYBlock = (y / BLOCK_LENGTH) * PIXEL_DENSITY2;
            iXBlock = (x / BLOCK_LENGTH);

            for (int b = 0; b < BLOCK_LENGTH * BLOCK_LENGTH; b++) {
                iYPlacement = b / BLOCK_LENGTH;
                iXPlacement = b % BLOCK_LENGTH;

                rawPixelPosition = (y + iYPlacement) * SDL2_WIDTH + iXPlacement + x;

                pixels[iYBlock + iXBlock].pixels[b] = &rawPixels[rawPixelPosition];
            }
            if ((iYBlock + iXBlock) % 2 == 0) {
                makeColorForPixel(&pixels[iYBlock + iXBlock], RED, HIGH);
            } else {
                makeColorForPixel(&pixels[iYBlock + iXBlock], GREEN, HIGH);
            }
        }
    }

    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB332);
    //uint8_t *palette = createPalette(format);

    uint8_t something = 0;
    uint8_t something2 = 0;
    makeColor(RED, &something, HIGH);
    makeColor(GREEN, &something, LOWER);
    makeColor(BLUE, &something, MAX);

    makeColorRGB(&something2, HIGH, ZERO, ZERO);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Update texture and render
        SDL_UpdateTexture(texture, NULL, rawPixels, SDL2_WIDTH * sizeof(uint8_t));
        //SDL_UpdateTexture(texture, NULL, pixels, SDL2_WIDTH * sizeof(uint8_t));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(pixels);
    return 0;
}

void makeColorForPixel(PIXEL *pixel, enum COLOR color, uint8_t colorValue) {
    makeColor(color, &pixel->color, colorValue);
    for (int i = 0; i < BLOCK_LENGTH * BLOCK_LENGTH; i++) {
        *pixel->pixels[i] = pixel->color;
    }
}

void makeColor(enum COLOR color, uint8_t *value, uint8_t colorValue) {
    uint8_t mask = 0, bitshift = 0;
    switch (color) {
        case RED:
            bitshift = 5;
            mask = 0x07;
            break;
        case GREEN:
            bitshift = 2;
            mask = 0x07;
            break;
        case BLUE:
            bitshift = 0;
            mask = 0x03;
            break;
        default:
            return;
    }

    *value &= ~(mask << bitshift);
    *value |= (colorValue & mask) << bitshift;
}

void makeColorRGB(uint8_t *value, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t mask = 0, bitshift = 0;

    // RED
    mask = 0x07;
    bitshift = 5;
    *value &= ~(mask << bitshift);
    *value |= (red & mask) << bitshift;

    // GREEN
    mask = 0x07;
    bitshift = 2;
    *value &= ~(mask << bitshift);
    *value |= (green & mask) << bitshift;

    // BLUE
    mask = 0x03;
    bitshift = 0;
    *value &= ~(mask << bitshift);
    *value |= (blue & mask) << bitshift;
}

void writeToFile(const STE_FILE *steFile, FILE *file) {
    fputs(steFile->magicValue, file);
    fputc(steFile->width, file);
    fputc(steFile->height, file);
    fwrite(steFile->body, 1, steFile->width * steFile->height, file);
    //fputs(steFile->body, file);
    fclose(file);
}

int8_t widthAndHeightChecks(const unsigned int iHeightLength,
                            const unsigned int iWidthLength,
                            const char *pszHeight,
                            const char *pszWidth) {
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
