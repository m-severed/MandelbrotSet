//
// Created by Philip Tran on 25.06.2023.
//
#include <complex.h>
#include "mandelbrot.h"

void mandelbrot_V1(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img) {
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            // calculation of the constant c
            float complex c = (crealf(start) + res * x) + I * (cimagf(start) + res * y);
            // starting value z = 0
            float complex z = 0;
            unsigned iterations = 0;

            // Getting the number of iterations
            while ((crealf(z) * crealf(z) + cimagf(z) * cimagf(z) <= 4.0) && iterations < n) {
                // verifying if the current magnitude is smaller or equal to 4 and the number of iterations is smaller than n
                z = z * z + c;
                iterations++;
                // calculating the next z and incrementing the iterations
            }

            unsigned char *pixel = &img[(y * width + x) * 3];
            if (iterations == n) {
                // if we reached the maximum number of iterations, set the pixel to the black color
                pixel[0] = 0;  // Blue channel
                pixel[1] = 0;  // Green channel
                pixel[2] = 0;  // Red channel
            } else {
                pixel[0] = 255 * iterations / (n - 1);
                if (pixel[0] == 255) {
                    pixel[1] = 0;
                    pixel[2] = 0;
                } else {
                    pixel[1] = (255 * 255 * iterations / (n - 1)) % 256;
                    if (pixel[1] == 255) {
                        pixel[0] = 0;
                        pixel[2] = 0;
                    } else {
                        pixel[2] = (255 * 255 * 255 * iterations / (n - 1)) % 128 + 128;
                        if (pixel[2] == 255) {
                            pixel[0] = 0;
                            pixel[1] = 0;
                        }
                    }
                }
                // Setting the pixels using simple arithmetic based on the number of iterations
            }
        }
    }
}

