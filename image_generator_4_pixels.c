//
// Created by User on 09/07/2023.
//

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "image_generator_4_pixels.h"


// This file is exactly the same as the image_generator.c file. Except for a few lines, refer to the
// comments in the BMP_4P function, where the bitDepth is changed to 32 instead of 24 and the image
// writing for-loop is slightly adjusted.
typedef struct {
    char signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
} __attribute__((packed)) BitmapFileHeader;
/*
 * When working with the BMP file format, it is common to ensure that the structs are
 * packed tightly against each other in memory. Strictly speaking, it is not necessary
 * but if absent, the compiler may introduce padding itself between structure members
 * for alignment purposes. It simply allows for more efficient access.
 */
typedef struct {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitDepth;
    uint32_t compression;
    uint32_t imageSize;
    int32_t horizontalRes;
    int32_t verticalRes;
    uint32_t colors;
    uint32_t importantColors;
} __attribute__((packed)) BitmapInfoHeader;


void BMP_4P(const char *filename, unsigned char *imageData, int32_t width, int32_t height) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        int32_t paddingSize = (4 - (width * 4) % 4) % 4;
        uint32_t imageDataSize = (uint32_t)((width * 4 + paddingSize) * height);
        uint32_t fileSize = (uint32_t) (sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + imageDataSize);
        BitmapFileHeader fileHeader;
        fileHeader.signature[0] = 'B';
        fileHeader.signature[1] = 'M';
        fileHeader.fileSize = fileSize;
        fileHeader.reserved1 = 0;
        fileHeader.reserved2 = 0;
        fileHeader.dataOffset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
        fwrite(&fileHeader, sizeof(fileHeader), 1, file);

        BitmapInfoHeader infoHeader;
        infoHeader.headerSize = sizeof(BitmapInfoHeader);
        infoHeader.width = width;
        infoHeader.height = height;
        infoHeader.planes = 1;
        infoHeader.bitDepth = 32; // Updated to 32 bits per pixel for RGBA
        infoHeader.compression = 0;
        infoHeader.imageSize = imageDataSize;
        infoHeader.horizontalRes = 0;
        infoHeader.verticalRes = 0;
        infoHeader.colors = 0;
        infoHeader.importantColors = 0;
        fwrite(&infoHeader, sizeof(infoHeader), 1, file);

        for (int32_t y = height - 1; y >= 0; y--) {
            for (int32_t x = 0; x < width; x++) {
                unsigned char *pixel = &imageData[(y * width + x) * 4]; // 4 bytes per pixel (RGBA)
                fwrite(pixel + 2, 1, 1, file);  // Blue channel
                fwrite(pixel + 1, 1, 1, file);  // Green channel
                fwrite(pixel, 1, 1, file);      // Red channel
                fwrite(pixel + 3, 1, 1, file);  // Alpha channel

            }

            for (int32_t i = 0; i < paddingSize; i++) {
                fputc(0, file);
            }
        }

        fclose(file);
    } else {
        printf("Error opening file for writing.\n");
        fclose(file);
    }
}

