#include "../include/steFile.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#define MAGIC_VALUE "STE0"
#define FILE_EXTENSION ".ste"
#define LENGTH_OF_EXTENSION 4
#define SDL2_WIDTH 255
#define SDL2_HEIGHT 255

typedef struct _STE_FILE {
    char magicValue[5];
    char width;
    char height;
} STE_FILE;

void writeToFile(const STE_FILE *steFile, FILE *file);
int readSteFile();

int8_t widthAndHeightChecks(unsigned int iHeightLength,
                            unsigned int iWidthLength,
                            const char *pszHeight,
                            const char *pszWidth);

int createFile(const char **argv) {
    int8_t iStatus = 0;
    const char *pszName = argv[2];
    const char *pszWidth = argv[3];
    const char *pszHeight = argv[4];
    const unsigned int iHeightLength = strlen(pszHeight);
    const unsigned int iWidthLength = strlen(pszWidth);
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

    steFile.height = atoi(pszHeight);
    steFile.width = atoi(pszWidth);

    iLengthOfName = strlen(pszName);

    if (iLengthOfName > LENGTH_OF_EXTENSION) {
        for (int i = 0; i < LENGTH_OF_EXTENSION; i++) {
            szCheckIfExtension[i] = pszName[iLengthOfName - LENGTH_OF_EXTENSION + i];
        }
        szCheckIfExtension[LENGTH_OF_EXTENSION] = '\0';

        checkIfExtensionValue = strncmp(szCheckIfExtension, FILE_EXTENSION, 4);
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
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SDL2_WIDTH, SDL2_HEIGHT);
    uint32_t pixels[SDL2_WIDTH * SDL2_HEIGHT];
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        for (int y = 0; y < SDL2_HEIGHT; y++) {
            for (int x = 0; x < SDL2_WIDTH; x++) {
                // Example: Gradient pattern
                uint8_t r = x;  // Red increases with X
                uint8_t g = y;  // Green increases with Y
                uint8_t b = 0;  // Blue is zero
                pixels[y * SDL2_WIDTH + x] = (r << 24) | (g << 16) | (b << 8) | 0xFF;
            }
        }

        // Update texture and render
        SDL_UpdateTexture(texture, NULL, pixels, SDL2_WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
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
