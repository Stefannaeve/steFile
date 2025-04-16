#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "../include/windowApplicationSteFile.h"
#include "../include/steFile.h"
#include "../include/SNLogger.h"

#define SDL2_WIDTH 1024
#define SDL2_HEIGHT 1024
#define PIXEL_DENSITY (SDL2_WIDTH/BLOCK_LENGTH)

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

void freePixels(PIXEL **pixels);

void changeColorsOfPixelsFromPicture(STE_PICTURE *stePicture);

int representPictureForSpecificCoordinates(STE_PICTURE *stePicture, int externalX, int externalY, PIXEL **pixels);

int populatePictureFromSteFile(STE_PICTURE *ste_picture, char *fileName);

void makeColorForPixel(PIXEL *pixel, enum COLOR color, uint8_t colorValue);

void makeColor(enum COLOR color, uint8_t *value, uint8_t colorValue);

void makeColorRGB(uint8_t *value, uint8_t red, uint8_t green, uint8_t blue);

void writeToFile(const STE_FILE *steFile, FILE *file);

int readSteFile();

int8_t widthAndHeightChecks(unsigned int iHeightLength,
                            unsigned int iWidthLength,
                            const char *pszHeight,
                            const char *pszWidth);

int readSteFile() {
    int iStatus = 0;

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
    PIXEL **pixels = calloc(PIXEL_DENSITY, sizeof(PIXEL));

    for (int i = 0; i < PIXEL_DENSITY; i++) {
        pixels[i] = calloc(PIXEL_DENSITY, sizeof(PIXEL));
    }

    int iYPlacement = 0;
    int iXPlacement = 0;
    int iYBlock = 0;
    int iXBlock = 0;
    int rawPixelPosition = 0;

    for (int y = 0; y < SDL2_WIDTH; y += BLOCK_LENGTH) {
        for (int x = 0; x < SDL2_WIDTH; x += BLOCK_LENGTH) {
            iYBlock = (y / BLOCK_LENGTH);
            iXBlock = (x / BLOCK_LENGTH);

            for (int b = 0; b < BLOCK_LENGTH * BLOCK_LENGTH; b++) {
                iYPlacement = b / BLOCK_LENGTH;
                iXPlacement = b % BLOCK_LENGTH;

                rawPixelPosition = (y + iYPlacement) * SDL2_WIDTH + iXPlacement + x;

                pixels[iYBlock][iXBlock].pixels[b] = &rawPixels[rawPixelPosition];
            }
            if ((iYBlock + iXBlock) % 2 == 0) {
                makeColorForPixel(&pixels[iYBlock][iXBlock], RED, HIGH);
            } else {
                makeColorForPixel(&pixels[iYBlock][iXBlock], GREEN, HIGH);
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


    STE_PICTURE snakeHeadOpen = {0};
    STE_PICTURE snakeHeadClosed = {0};
    STE_PICTURE snakeHeadSemiOpen = {0};
    char *imageName = "images/snakeHeadOpen.ste";
    char *imageName2 = "images/snakeHeadClosed.ste";
    char *imageName3 = "images/snakeHeadSemiOpen.ste";

    iStatus = populatePictureFromSteFile(&snakeHeadOpen, imageName);
    iStatus = populatePictureFromSteFile(&snakeHeadClosed, imageName2);
    iStatus = populatePictureFromSteFile(&snakeHeadSemiOpen, imageName3);

    if (iStatus != 0) {

    } else {
        iStatus = representPictureForSpecificCoordinates(&snakeHeadOpen, 30, 30, pixels);
        if (iStatus != 0) {

        } else {
            changeColorsOfPixelsFromPicture(&snakeHeadOpen);

            int running = 1;
            uint8_t placement = 0;
            struct timespec timespec = {0};
            timespec.tv_sec = 110 / 1000;
            timespec.tv_nsec = (110 % 1000) * 1000000;
            int xPosition = 0;
            while (running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = 0;
                    }
                }
                nanosleep(&timespec, NULL);
                if (placement == 0) {
                    iStatus = representPictureForSpecificCoordinates(&snakeHeadOpen, xPosition, 30, pixels);
                    changeColorsOfPixelsFromPicture(&snakeHeadOpen);
                } else if (placement == 1 || placement == 3) {
                    iStatus = representPictureForSpecificCoordinates(&snakeHeadSemiOpen, xPosition, 30, pixels);
                    changeColorsOfPixelsFromPicture(&snakeHeadSemiOpen);

                }else {
                    iStatus = representPictureForSpecificCoordinates(&snakeHeadClosed, xPosition, 30, pixels);
                    changeColorsOfPixelsFromPicture(&snakeHeadClosed);
                }
                if (xPosition + snakeHeadSemiOpen.width < PIXEL_DENSITY) {
                    xPosition++;
                }
                placement += 1;
                if (placement > 3) {
                    placement = 0;
                }

                // Update texture and render
                SDL_UpdateTexture(texture, NULL, rawPixels, SDL2_WIDTH * sizeof(uint8_t));
                //SDL_UpdateTexture(texture, NULL, pixels, SDL2_WIDTH * sizeof(uint8_t));
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
        }
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(snakeHeadOpen.pixel);
    freePixels(pixels);
    return iStatus;
}

void freePixels(PIXEL **pixels) {
    for (int i = 0; i < PIXEL_DENSITY; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

void changeColorsOfPixelsFromPicture(STE_PICTURE *stePicture) {
    for (int y = 0; y < stePicture->height; y++) {
        for (int x = 0; x < stePicture->width; x++) {
            for (int i = 0; i < BLOCK_LENGTH * BLOCK_LENGTH; i++) {
                *stePicture->pixel[y][x].pixels[i] = stePicture->pixel[y][x].color;
            }
        }
    }
}

int representPictureForSpecificCoordinates(STE_PICTURE *stePicture, int externalX, int externalY, PIXEL **pixels) {
    int iStatus = 0;
    if (externalX + stePicture->width > PIXEL_DENSITY) {
        iStatus = 1;
        logError(
            "Width placement set to: %d, which combined with the picture width of %d, would surpass window pixel width of %d",
            externalX, stePicture->width, PIXEL_DENSITY);
        return iStatus;
    }
    if (externalY + stePicture->height > PIXEL_DENSITY) {
        iStatus = 1;
        logError(
            "Height placement set to: %d, which combined with the picture height of %d, would surpass window pixel height of %d",
            externalX, stePicture->width, PIXEL_DENSITY);
        return iStatus;
    }
    int color = 0;

    for (int y = 0; y < stePicture->height; y++) {
        for (int x = 0; x < stePicture->width; x++) {
            color = stePicture->pixel[y][x].color;
            stePicture->pixel[y][x] = pixels[externalY + y][externalX + x];
            stePicture->pixel[y][x].color = color;
        }
    }
    return iStatus;
}

int populatePictureFromSteFile(STE_PICTURE *ste_picture, char *fileName) {
    int iStatus = 0;
    uint8_t value = 0;
    int numberOfPixels = 0;
    FILE *file;
    char fromFilePrefix[LENGTH_OF_EXTENSION + 1] = {0};

    file = fopen(fileName, "r");
    if (file == NULL) {
        iStatus = 1;
        logError("Failed to open file %s", fileName);
        return 1;
    }

    fread(fromFilePrefix, sizeof(char), LENGTH_OF_EXTENSION, file);
    if (strncmp(fromFilePrefix, MAGIC_VALUE, LENGTH_OF_EXTENSION) != 0) {
        iStatus = 1;
        logError("%s does not have the right prefix to classify it as a steFile", fileName);
        return iStatus;
    }

    fread(&value, sizeof(uint8_t), 1, file);
    ste_picture->width = value;

    fread(&value, sizeof(uint8_t), 1, file);
    ste_picture->height = value;

    numberOfPixels = ste_picture->width * ste_picture->height;

    ste_picture->pixel = calloc(ste_picture->height, sizeof(PIXEL));
    if (ste_picture->pixel == NULL) {
        iStatus = 1;
        logError(MEMORY_ALLOCATION);
        return iStatus;
    }

    for (int i = 0; i < ste_picture->height; i++) {
        ste_picture->pixel[i] = calloc(ste_picture->width, sizeof(PIXEL));
        if (ste_picture->pixel[i] == NULL) {
            iStatus = 1;
            logError(MEMORY_ALLOCATION);
            return iStatus;
        }
    }

    for (int y = 0; y < ste_picture->height; y++) {
        for (int x = 0; x < ste_picture->width; x++) {
            fread(&ste_picture->pixel[y][x].color, sizeof(uint8_t), 1, file);
        }
    }

    return iStatus;
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
