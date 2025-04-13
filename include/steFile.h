//
// Created by stefan on 2/1/25.
//

#ifndef __STEFILE_H
#define __STEFILE_H

#include <stdint.h>

#define MAGIC_VALUE "STE0"
#define FILE_EXTENSION ".ste"
#define LENGTH_OF_EXTENSION 4

typedef struct _STE_FILE {
    uint8_t magicValue[5];
    uint8_t width;
    uint8_t height;
    uint8_t *body;
} STE_FILE;

#endif
