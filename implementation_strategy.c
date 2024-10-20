//
// Created by Philip Tran on 28.06.2023.
//

#include <stdint.h>
#include <complex.h>

#include "implementation_strategy.h"
#include "../mandelbrot_calculations/mandelbrot.h"
#include "../mandelbrot_calculations/mandelbrot_firstOptimization.h"
#include "../mandelbrot_calculations/mandelbrot_simd.h"

// based on Strategy pattern helper method to choose the correct implementation
void mandelbrotStrategy(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img,
                        unsigned implementation) {
    switch (implementation) {
        case 0:
            mandelbrot(start, width, height, res, n, img);
            break;
        case 1:
            mandelbrot_V1(start, width, height, res, n, img);
            break;
        case 2:
            mandelbrot_V2(start, width, height, res, n, img);
            break;
        default:
            return;
    }
}
