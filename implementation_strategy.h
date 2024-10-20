//
// Created by Philip Tran on 28.06.2023.
//
#include <complex.h>
#include <stdio.h>

#ifndef MANDELBROT_MENGE_IMPLEMENTATION_STRATEGY_H
#define MANDELBROT_MENGE_IMPLEMENTATION_STRATEGY_H

void mandelbrotStrategy(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img,
                        unsigned implementation);

#endif //MANDELBROT_MENGE_IMPLEMENTATION_STRATEGY_H
