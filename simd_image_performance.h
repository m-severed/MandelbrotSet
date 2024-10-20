//
// Created by Philip Tran on 10.07.2023.
//
#include <complex.h>
#include <stdint.h>
#include <stddef.h>

#ifndef MANDELBROT_MENGE_SIMD_IMAGE_PERFORMANCE_H
#define MANDELBROT_MENGE_SIMD_IMAGE_PERFORMANCE_H

void mandelbrot_simd_optimization_new_image(float complex start, int32_t width, int32_t height, float res, unsigned n,
                                            unsigned char *img);

#endif //MANDELBROT_MENGE_SIMD_IMAGE_PERFORMANCE_H
