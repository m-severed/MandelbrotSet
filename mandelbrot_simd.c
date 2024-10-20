//
// Created by User on 28/06/2023.
//
#include <complex.h>
#include <stdint.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include "mandelbrot_simd.h"

void mandelbrot(float complex start, int32_t width, int32_t height, float res, unsigned n,
                unsigned char *img) {

    unsigned int lookupTable[16] = {0xFF0F1E42, 0xFF200000, 0xFF2F0109, 0xFF490404, 0xFF640700, 0xFF8A2C0C, 0xFFB15218,
                                    0xFFD17D39, 0xFFE5B586, 0xFFF8ECD3, 0xFFBFE9F1, 0xFF5FC9F8, 0xFF00AAFF, 0xFF0080CC,
                                    0xFF005799, 0xFF03346A};
    // initialising the lookup-table for the sequential part
    unsigned char lookupTable1[16][4] = {{66,  30,  15,  255},
                                         {0,   0,   32,  255},
                                         {9,   1,   47,  255},
                                         {4,   4,   73,  255},
                                         {0,   7,   100, 255},
                                         {12,  44,  138, 255},
                                         {24,  82,  177, 255},
                                         {57,  125, 209, 255},
                                         {134, 181, 229, 255},
                                         {211, 236, 248, 255},
                                         {241, 233, 191, 255},
                                         {248, 201, 95,  255},
                                         {255, 170, 0,   255},
                                         {204, 128, 0,   255},
                                         {153, 87,  0,   255},
                                         {106, 52,  3,   255}};
    // initialising the lookup-table for SIMD

    float cReal, cImaginary = cimagf(start), zReal, zImaginary, copyZReal, copyZImaginary;
    __m128i defaultPixel = _mm_set1_epi32(
            (int) 0xFF000000), mask_for_equality, negated_mask, _iterations, unsigned_n, unsigned_iterations, one = _mm_set1_epi32(
            1), iterations_offset, final_mask;
    int32_t y_offset = 0, x, y;
    __m128 magnitude, four = _mm_set1_ps(
            4.0f), mask_for_magnitude, zReal_SIMD, zImaginary_SIMD, copy_zReal_SIMD, copy_zImaginary_SIMD, cImaginary_SIMD, temp, four_of_res = _mm_set_ps(
            0, 1, 2, 3), cReal_SIMD;
    __m128i _n = _mm_set1_epi32((int) n);
    __m128 _res = _mm_set1_ps(res);
    __m128 jump = _mm_set1_ps(res * 4);
    four_of_res = _mm_mul_ps(four_of_res, _res);

    for (y = 0; y < height; y++) {
        // SIMD loop
        cReal = crealf(start);
        cReal_SIMD = _mm_add_ps(_mm_set1_ps(crealf(start)), four_of_res);
        // initialising four real parts of the c constant which are consecutively
        cImaginary_SIMD = _mm_set1_ps(cImaginary);
        // initialising the imaginary component
        for (x = 0; x + 4 <= width; x = x + 4) {
            zReal_SIMD = _mm_set1_ps(0);
            copy_zReal_SIMD = _mm_set1_ps(0);
            copy_zImaginary_SIMD = _mm_set1_ps(0);
            zImaginary_SIMD = _mm_set1_ps(0);
            // starting value z = 0
            _iterations = _mm_set1_epi32(1);
            // initial value of iterations, since we are beginning with calculating the next z in the loop
            do {
                temp = _mm_mul_ps(zReal_SIMD, zImaginary_SIMD);

                zImaginary_SIMD = _mm_add_ps(_mm_add_ps(temp, temp), cImaginary_SIMD);
                zReal_SIMD = _mm_add_ps(_mm_sub_ps(copy_zReal_SIMD, copy_zImaginary_SIMD), cReal_SIMD);
                // setting the new values for zReal_SIMD and ZImaginary_SIMD
                copy_zReal_SIMD = _mm_mul_ps(zReal_SIMD, zReal_SIMD);
                copy_zImaginary_SIMD = _mm_mul_ps(zImaginary_SIMD, zImaginary_SIMD);
                magnitude = _mm_add_ps(copy_zReal_SIMD, copy_zImaginary_SIMD);
                // calculating the magnitude of z
                mask_for_magnitude = _mm_cmple_ps(magnitude, four);
                // using a bit mask for comparing the magnitude of z with 4
                unsigned_iterations = _mm_add_epi32(_iterations, _mm_set1_epi32(INT32_MIN));
                unsigned_n = _mm_add_epi32(_n, _mm_set1_epi32(INT32_MIN));
                final_mask = _mm_cmpgt_epi32(unsigned_n, unsigned_iterations);
                // making sure you compare the unsigned values of n and iterations
                final_mask = _mm_and_si128(final_mask, (__m128i) (mask_for_magnitude));
                // bitwise and between the magnitude_mask and the mask comparing the current number of iterations with n
                iterations_offset = _mm_and_si128(one, final_mask);
                // the exact number that we need to add to the 4 numbers of iterations
                _iterations = _mm_add_epi32(_iterations, iterations_offset);
                // adding the correct offset to the iterations

            } while (_mm_movemask_epi8(final_mask) != 0);
            // verifying if the final_mask is 0, to know if we have to stop incrementing the iterations
            unsigned char *pixel = &img[(y_offset + x) * 4];

            mask_for_equality = _mm_cmpeq_epi32(_n, _iterations);
            // verifying which from our 4 iterations are equal to n
            mask_for_equality = _mm_set_epi32((int) _mm_extract_epi32(mask_for_equality, 0),
                                              (int) _mm_extract_epi32(mask_for_equality, 1),
                                              (int) _mm_extract_epi32(mask_for_equality, 2),
                                              (int) _mm_extract_epi32(mask_for_equality, 3));
            // rearranging the mask_for_equality in order to match the order of the data that we are going to store
            __m128i mod16 = _mm_and_si128(_iterations, _mm_set1_epi32(0x0F));
            // doing modulo 16 on the number of iterations to get the index from the lookup-table
            __m128i dataVec = _mm_set_epi32((int) lookupTable[_mm_extract_epi32(mod16, 0)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 1)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 2)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 3)]);
            // get the data from the lookup-table in parallel
            negated_mask = ~(mask_for_equality);
            dataVec = _mm_and_si128(dataVec, negated_mask);
            // setting the data, where the number of iterations is equal to n to zero
            dataVec = _mm_or_si128(dataVec, _mm_and_si128(defaultPixel, mask_for_equality));
            // storing the black pixel where we have the number of iterations is equal to n
            _mm_storeu_si128((__m128i *) pixel, dataVec);
            // storing the pixels data
            cReal_SIMD = _mm_add_ps(cReal_SIMD, jump);
            cReal = cReal + res * 4;
            // incrementing the real part of c
        }
        for (; x < width; x++) {
            // sequential loop if the width is not a multiple of 4
            zReal = 0, zImaginary = 0;
            // starting value z = 0
            copyZReal = 0, copyZImaginary = 0;
            unsigned iterations = 0;
            // Getting the number of iterations
            while (copyZReal + copyZImaginary <= 4.0 && iterations < n) {
                // verifying if the current magnitude is smaller or equal to 4 and the number of iterations is smaller than n
                zImaginary = 2 * zReal * zImaginary + cImaginary;
                zReal = copyZReal - copyZImaginary + cReal;
                // setting the new values for zReal and ZImaginary
                copyZReal = zReal * zReal;
                copyZImaginary = zImaginary * zImaginary;
                iterations++;
                // incrementing the iterations
            }
            unsigned char *pixel = &img[(y_offset + x) * 4];

            if (iterations == n) {
                // if we reached the maximum number of iterations, set the pixel to the black color
                pixel[0] = 0;  // Blue channel
                pixel[1] = 0;  // Green channel
                pixel[2] = 0;  // Red channel
                pixel[3] = 255;  // Alpha channel
            } else {
                iterations = iterations % 16;
                pixel[0] = lookupTable1[iterations][0];
                pixel[1] = lookupTable1[iterations][1];
                pixel[2] = lookupTable1[iterations][2];
                pixel[3] = lookupTable1[iterations][3];
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

