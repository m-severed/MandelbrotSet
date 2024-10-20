//
// Created by Philip Tran on 10.07.2023.
//

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

#ifndef MANDELBROT_MENGE_OPTIMIZED_SISD_PERFORMANCE_IMAGE_H
#define MANDELBROT_MENGE_OPTIMIZED_SISD_PERFORMANCE_IMAGE_H

void
mandelbrot_optimized_image(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img);

#endif //MANDELBROT_MENGE_OPTIMIZED_SISD_PERFORMANCE_IMAGE_H
