//
// Created by Philip Tran on 09.07.2023.
//
#include <complex.h>
#include <stdint.h>

void default_mandelbrot_image(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img) {


    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            // calculation of the constant C
            float complex c = (crealf(start) + res * x) + I * (cimagf(start) + res * y);
            // starting value z = 0
            float complex z = 0;
            unsigned iterations = 0;

            // Getting the number of iterations
            while ((crealf(z) * crealf(z) + cimagf(z) * cimagf(z) <= 4) && iterations < n) {
                z = z * z + c;
                iterations++;
            }

            unsigned char *pixel = &img[(y * width + x) * 3];
            if (iterations == n) {
                pixel[0] = 0;  // Blue channel
                pixel[1] = 0;    // Green channel
                pixel[2] = 0;
            } else {
                pixel[0] = 255 * iterations / (n - 1);
                if (pixel[0] == 255) {
                    pixel[1] = 0;
                    pixel[2] = 0;
                } else {
                    pixel[1] = (255 * 255 * iterations / (n - 1)) % 256;
                    if (pixel[1] == 255) pixel[2] = 0;
                    else {
                        pixel[2] = (255 * 255 * 255 * iterations / (n - 1)) % 128 + 128;
                        if (pixel[0] == 255) {
                            pixel[1] = 0;
                            pixel[2] = 0;
                        }
                    }
                }
            }
        }
    }
}
