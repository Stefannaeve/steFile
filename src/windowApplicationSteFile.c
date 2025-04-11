#include "../include/steFile.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#define SDL2_WIDTH 1024
#define SDL2_HEIGHT 1024
#define PIXEL_DENSITY 512
#define BLOCK_LENGTH 16
#define PIXEL_DENSITY2 (SDL2_WIDTH/BLOCK_LENGTH)

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
