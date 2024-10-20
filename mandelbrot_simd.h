//
// Created by User on 28/06/2023.
//
#include <complex.h>
#include <stddef.h>
#include <smmintrin.h>
#include <stdint.h>


#ifndef MANDELBROT_MENGE_MANDLEBROT_SIMD_H
#define MANDELBROT_MENGE_MANDLEBROT_SIMD_H

void mandelbrot(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img);

#endif //MANDELBROT_MENGE_MANDLEBROT_SIMD_H
