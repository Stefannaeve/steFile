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
    uint8_t *pixels[4];
    uint8_t color;
} PIXEL;

typedef struct _STE_FILE {
    char magicValue[5];
    char width;
    char height;
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

    iStatus = widthAndHeightChecks(iHeightLength, iWidthLength, pszHeight, pszWidth);

    if (iStatus != 0) {
        return 1;
    }

    memcpy(steFile.magicValue, MAGIC_VALUE, 4);
    steFile.magicValue[4] = '\0';

    steFile.height = atoi(pszHeight); // mmmm
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

int readTextFile(const char *pszName) {
    int iStatus = 0;
    int position = 0;
    char buffer[BUFFER_SIZE] = {0};
    char *folderName = "rawImageFiles/";
    int iNameLength = strlen(pszName);
    int iFolderNameLength = strlen(folderName);
    char *name = calloc(iNameLength + iFolderNameLength + 1, sizeof(
        char));

    strncat(name, folderName, iFolderNameLength);
    strncat(name, pszName, iNameLength);

    FILE *file = fopen(name, "r");
    logDebug("Hei TextFile: 1");
    if (file == NULL) {
        iStatus = 1;
        logDebug("Hei TextFile: 2");
        return iStatus;
    }



    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        logError("Buffer is null, failed fgets");
        iStatus = 1;
        logDebug("Hei TextFile: 3");
        return iStatus;
    }

    logDebug("Buffer: %s", buffer);

    free(name);
    return 0;

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

    /*
    steFile.height = atoi(pszHeight); // mmmm
    steFile.width = atoi(pszWidth);
    */

    steFile.height = 16; // mmmm
    steFile.width = 16;

    readTextFile(pszName);

    iLengthOfName = strlen(pszName);

    if (iLengthOfName > LENGTH_OF_EXTENSION) {
        for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
            szCheckIfExtension[i] = pszName[iLengthOfName - LENGTH_OF_EXTENSION + i];
        }
        szCheckIfExtension[LENGTH_OF_EXTENSION] = '\0';

        checkIfExtensionValue = strncmp(szCheckIfExtension, FILE_EXTENSION, 4);
    }

    if (checkIfExtensionValue != 0) {
        pszFileName = (char *) calloc(iLengthOfName + imageFolderLength + LENGTH_OF_EXTENSION + 1,
                                      sizeof(char));

        if (pszFileName == NULL) {
            iStatus = 1;
            printf("Memory allocation issue");
        } else {
            strncat(pszFileName, imageFolder, imageFolderLength);
            strncat(pszFileName, pszName, iLengthOfName);
            strncat(pszFileName, FILE_EXTENSION, LENGTH_OF_EXTENSION + 1);
            pszFileName[iLengthOfName + LENGTH_OF_EXTENSION + imageFolderLength] = '\0';
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
    logDebug("4"); // Dårlig log melding
    SDL_Init(SDL_INIT_VIDEO);
    logDebug("5"); // ^

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
    PIXEL *pixels = malloc(PIXEL_DENSITY * PIXEL_DENSITY * sizeof(PIXEL));
    int pixelPosition = 0;
    for (int y = 0; y < SDL2_WIDTH; y++) {
        for (int x = 0; x < SDL2_HEIGHT - 1; x++) {
            if (y % 2 == 0) {
                if (x % 2 == 0) {
                    const int topLeftIndex = y * SDL2_WIDTH + x;
                    const int topRightIndex = y * SDL2_WIDTH + (x + 1);
                    const int bottomLeftIndex = (y + 1) * SDL2_WIDTH + x;
                    const int bottomRightIndex = (y + 1) * SDL2_WIDTH + x + 1;

                    pixels[pixelPosition].pixels[0] = &rawPixels[topLeftIndex];
                    pixels[pixelPosition].pixels[1] = &rawPixels[topRightIndex];
                    pixels[pixelPosition].pixels[2] = &rawPixels[bottomLeftIndex];
                    pixels[pixelPosition].pixels[3] = &rawPixels[bottomRightIndex];
                    makeColorForPixel(&pixels[pixelPosition], RED, LOW);

                    pixelPosition++;
                }
            } else {
                break;
            }
        }
    }

    pixelPosition = 0;

    for (int y = 0; y < PIXEL_DENSITY; y = y + 2) {
        for (int x = 0; x < PIXEL_DENSITY - 1; x = x + 2) {
            makeColorForPixel(&pixels[y * PIXEL_DENSITY + x], GREEN, LOW);
            pixelPosition++;
        }
    }


    /*
    for (int i = 0; i < PIXEL_DENSITY; i++) {
        if (i % 2 == 0) {
            makeColor(RED, &pixels->color, MEDIUM);
            makeColorForPixel(&pixels[pixelPosition], RED, LOW);
        }
    }
    */


    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB332);
    //uint8_t *palette = createPalette(format);

    uint8_t something = 0;
    uint8_t something2 = 0;
    makeColor(RED, &something, HIGH);
    makeColor(GREEN, &something, LOWER);
    makeColor(BLUE, &something, MAX);

    makeColorRGB(&something2, HIGH, ZERO, ZERO);

    printf("something: %d\n", something);
    printf("something2: %d\n", something2);

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        /*
        for (int y = 0; y < SDL2_HEIGHT; y++) {
            for (int x = 0; x < SDL2_WIDTH; x++) {
                uint8_t mappedColor = 0x00;

                rawPixels[y * SDL2_WIDTH + x] = mappedColor;
            }
        }
        */

        /*
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                rawPixels[((SDL2_HEIGHT / 2 + i) * SDL2_WIDTH) + SDL2_WIDTH / 2 + j] = something2;
            }
        }
        */

        /*
        for (int y = 0; y < PIXEL_DENSITY; y++) {
            for (int x = 0; x < PIXEL_DENSITY; x++) {
                //if (j % 2 == 0) {
                int index = y * PIXEL_DENSITY + x;
                    pixels[index].pixels[0] = something;
                    pixels[index].pixels[1] = something;
                    pixels[index].pixels[2] = something;
                    pixels[index].pixels[3] = something;
                //}
            }
        }
        */

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
    for (int i = 0; i < 4; i++) {
        *pixel->pixels[i] = pixel->color;
        //logDebug("pixel %d: %d", i, *pixel->pixels[i]);
    }
    //logDebug("Color: %d", pixel->color);
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
    fputc('\n', file);
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
