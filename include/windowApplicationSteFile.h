#ifndef WINDOWAPPLICATIONSTEFILE_H
#define WINDOWAPPLICATIONSTEFILE_H

#define BLOCK_LENGTH 16

int readSteFile();

typedef struct _PIXEL {
    uint8_t *pixels[BLOCK_LENGTH * BLOCK_LENGTH];
    uint8_t color;
} PIXEL;

typedef struct _STE_PICTURE {
    uint8_t width;
    uint8_t height;
    PIXEL *pixel;
} STE_PICTURE;

#endif
