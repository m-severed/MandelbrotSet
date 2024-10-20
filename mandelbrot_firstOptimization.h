//
// Created by Philip Tran on 26.06.2023.
//

#include <complex.h>
#include <stddef.h>
#include <stdint.h>

#ifndef MANDELBROT_MENGE_MANDLEBROT_FIRSTOPTIMIZATION_H
#define MANDELBROT_MENGE_MANDLEBROT_FIRSTOPTIMIZATION_H

void mandelbrot_V2(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img);

#endif //MANDELBROT_MENGE_MANDLEBROT_FIRSTOPTIMIZATION_H
