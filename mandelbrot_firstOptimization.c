//
// Created by Philip Tran on 26.06.2023.
//

#include <complex.h>
#include "mandelbrot_firstOptimization.h"

void mandelbrot_V2(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img) {
    unsigned char lookupTable[16][3] = {{66,  30,  15},
                                        {0,   0,   32},
                                        {9,   1,   47},
                                        {4,   4,   73},
                                        {0,   7,   100},
                                        {12,  44,  138},
                                        {24,  82,  177},
                                        {57,  125, 209},
                                        {134, 181, 229},
                                        {211, 236, 248},
                                        {241, 233, 191},
                                        {248, 201, 95},
                                        {255, 170, 0},
                                        {204, 128, 0},
                                        {153, 87,  0},
                                        {106, 52,  3}};
    // initialising the lookup-table
    float cReal, cImaginary = cimagf(start), zReal, zImaginary, zReal2, zImaginary2;
    int32_t y_offset = 0;

    for (int32_t y = 0; y < height; y++) {
        cReal = crealf(start);
        for (int32_t x = 0; x < width; x++) {
            zReal = 0, zImaginary = 0;
            // starting value z = 0
            zReal2 = 0, zImaginary2 = 0;
            unsigned iterations = 0;
            // Getting the number of iterations
            while (zReal2 + zImaginary2 <= 4.0 && iterations < n) {
                // verifying if the current magnitude is smaller or equal to 4 and the number of iterations is smaller than n
                zImaginary = (zReal + zReal) * zImaginary + cImaginary;
                zReal = zReal2 - zImaginary2 + cReal;
                // setting the new values for zReal and ZImaginary
                zReal2 = zReal * zReal;
                zImaginary2 = zImaginary * zImaginary;
                iterations++;
                // incrementing the iterations
            }

            unsigned char *pixel = &img[(y_offset + x) * 3];
            if (iterations == n) {
                // if we reached the maximum number of iterations, set the pixel to the black color
                pixel[0] = 0;  // Blue channel
                pixel[1] = 0;  // Green channel
                pixel[2] = 0;  // Red channel
            } else {
                iterations = iterations % 16;
                pixel[0] = lookupTable[iterations][0];
                pixel[1] = lookupTable[iterations][1];
                pixel[2] = lookupTable[iterations][2];
                // Setting the pixels based on the lookup-table
            }

            cReal = cReal + res;
            // incrementing the real part of c
        }
        cImaginary = cImaginary + res;
        // incrementing the imaginary part of c
        y_offset = y_offset + width;
    }

}