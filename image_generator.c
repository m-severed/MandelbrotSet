//
// Created by Philip Tran on 25.06.2023.
//

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include "image_generator.h"

typedef struct {
    char signature[2];      // Used to identify the BMP and DIB file type (Use "BM" here)
    uint32_t fileSize;      // Size of the entire file
    uint16_t reserved1;     // Manually created images set this field to 0
    uint16_t reserved2;     // Manually created images set this field to 0
    uint32_t dataOffset;    // Offset to the actual image data
} __attribute__((packed)) BitmapFileHeader;

typedef struct {
    uint32_t headerSize;       // Size of the DIB header in bytes (40 bytes for BITMAPINFOHEADER)
    int32_t width;             // Bitmap width in pixels (Note that it's a signed integer)
    int32_t height;            // Bitmap height in pixels (Note that it's a signed integer)
    uint16_t planes;           // Number of colour planes (always set this to 1)
    uint16_t bitDepth;         // Number of BITS per pixel (24 for RGB and 32 for RGBA)
    uint32_t compression;      // Compression method, set this to zero, we don't want compressed images
    uint32_t imageSize;         // Size of the image data in bytes (including padding)
    int32_t horizontalRes;     // The horizontal resolution of the image (pixel per metre, signed integer)
    int32_t verticalRes;       // The vertical resolution of the image (pixel per metre, signed integer)
    uint32_t colors;           // Number of colors in the color palette (0 for full color)
    uint32_t importantColors;  // Number of important colors used, input 0 since every colour is important
}__attribute__((packed)) BitmapInfoHeader;

void BMP(const char *filename, unsigned char *imageData, int32_t width, int32_t height) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {

        int32_t paddingSize  = (4 - (width * 3) % 4) % 4;
        uint32_t imageDataSize = (uint32_t)((width * 3 + paddingSize) * height);
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
        infoHeader.bitDepth = 24;
        infoHeader.compression = 0;
        infoHeader.imageSize = imageDataSize;
        infoHeader.horizontalRes = 0;
        infoHeader.verticalRes = 0;
        infoHeader.colors = 0;
        infoHeader.importantColors = 0;
        fwrite(&infoHeader, sizeof(infoHeader), 1, file);

        for (int32_t y = height - 1; y >= 0; y--) {
            for (int32_t x = 0; x < width; x++) {
                unsigned char *pixel = &imageData[(y * width + x) * 3];
                fwrite(pixel + 2, 1, 1, file);  // Blue channel
                fwrite(pixel + 1, 1, 1, file);  // Green channel
                fwrite(pixel, 1, 1, file);      // Red channel

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

