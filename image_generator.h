//
// Created by Philip Tran on 25.06.2023.
//

#include <stddef.h>
#include <stdint.h>

#ifndef MANDELBROT_MENGE_IMAGE_GENERATOR_H
#define MANDELBROT_MENGE_IMAGE_GENERATOR_H

void BMP(const char *filename, unsigned char *imageData, int32_t width, int32_t height);

#endif //MANDELBROT_MENGE_IMAGE_GENERATOR_H
