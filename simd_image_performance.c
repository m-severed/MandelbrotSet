//
// Created by Philip Tran on 09.07.2023.
//

#include <complex.h>
#include <stdint.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <time.h>
#include <stdio.h>

void mandelbrot_simd_optimization_new_image(float complex start, int32_t width, int32_t height, float res, unsigned n,
                                            unsigned char *img) {

    unsigned int lookupTable[16] = {0xFF0F1E42, 0xFF1A0719, 0xFF2F0109, 0xFF490404, 0xFF640700, 0xFF8A2C0C, 0xFFB15218,
                                    0xFFD17D39, 0xFFE5B586, 0xFFF8ECD3, 0xFFBFE9F1, 0xFF5FC9F8, 0xFF00AAFF, 0xFF0080CC,
                                    0xFF005799, 0xFF03346A};
    unsigned char lookupTable1[16][4] = {{66,  30,  15,  255},
                                         {25,  7,   26,  255},
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
    float cReal, cImaginary = cimagf(start), zReal = 0, zImaginary, zReal2, zImaginary2;
    __m128i defaultPixel = _mm_set_epi32((int) 0xFF000000, (int) 0xFF000000, (int) 0xFF000000,
                                         (int) 0xFF000000), mask_for_equality, _negated_mask;
    int32_t y_offset = 0, x, y;
    __m128 _a, _four, _mask1;
    __m128 _zReal, _zImaginary, _copyZReal, _copyImaginary, _cImaginary, inter;
    __m128 res_offsets, _cReal, _res, _jump;
    __m128i _one, _c, _mask2;
    __m128i _n, _iterations, _signed_n, _signed_iterations;
    _one = _mm_set1_epi32(1);
    _four = _mm_set1_ps(4.0f);
    _n = _mm_set1_epi32((int) n);
    _res = _mm_set1_ps(res);
    _jump = _mm_set1_ps(res * 4);
    res_offsets = _mm_set_ps(0, 1, 2, 3);
    res_offsets = _mm_mul_ps(res_offsets, _res);
    for (y = 0; y < height; y++) {

        cReal = crealf(start);

        _a = _mm_set1_ps(crealf(start));
        _cReal = _mm_add_ps(_a, res_offsets);
        _cImaginary = _mm_set1_ps(cImaginary);
        for (x = 0; x < width; x = x + 4) {
            _zReal = _mm_set1_ps(0);
            _copyZReal = _mm_set1_ps(0);
            _copyImaginary = _mm_set1_ps(0);
            _zImaginary = _mm_set1_ps(0);
            _iterations = _mm_set1_epi32(1);
            do {
                inter = _mm_mul_ps(_zReal, _zImaginary);

                _zImaginary = _mm_add_ps(_mm_add_ps(inter, inter), _cImaginary);
                _zReal = _mm_add_ps(_mm_sub_ps(_copyZReal, _copyImaginary), _cReal);

                _copyZReal = _mm_mul_ps(_zReal, _zReal);
                _copyImaginary = _mm_mul_ps(_zImaginary, _zImaginary);

                _a = _mm_add_ps(_copyZReal, _copyImaginary);
                _mask1 = _mm_cmple_ps(_a, _four);

                _signed_iterations = _mm_add_epi32(_iterations, _mm_set1_epi32(INT32_MIN));
                _signed_n = _mm_add_epi32(_n, _mm_set1_epi32(INT32_MIN));
                _mask2 = _mm_cmpgt_epi32(_signed_n, _signed_iterations);

                _mask2 = _mm_and_si128(_mask2, (__m128i) (_mask1));
                _c = _mm_and_si128(_one, _mask2);
                _iterations = _mm_add_epi32(_iterations, _c);

            } while (_mm_movemask_epi8(_mask2) != 0);
            unsigned char *pixel = &img[(y_offset + x) * 4];

            mask_for_equality = _mm_cmpeq_epi32(_n, _iterations);
            mask_for_equality = _mm_set_epi32((int) _mm_extract_epi32(mask_for_equality, 0),
                                              (int) _mm_extract_epi32(mask_for_equality, 1),
                                              (int) _mm_extract_epi32(mask_for_equality, 2),
                                              (int) _mm_extract_epi32(mask_for_equality, 3));
            __m128i mod16 = _mm_and_si128(_iterations, _mm_set1_epi32(0x0F));
            __m128i dataVec = _mm_set_epi32((int) lookupTable[_mm_extract_epi32(mod16, 0)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 1)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 2)],
                                            (int) lookupTable[_mm_extract_epi32(mod16, 3)]);
            _negated_mask = ~(mask_for_equality);
            dataVec = _mm_and_si128(dataVec, _negated_mask);
            dataVec = _mm_or_si128(dataVec, _mm_and_si128(defaultPixel, mask_for_equality));
            _mm_storeu_si128((__m128i *) pixel, dataVec);

            _cReal = _mm_add_ps(_cReal, _jump);
            cReal = cReal + res * 4;
        }
        for (; x < width; x++) {
            zReal = 0, zImaginary = 0;
            zReal2 = 0, zImaginary2 = 0;
            unsigned iterations = 0;
            while (zReal2 + zImaginary2 <= 4.0 && iterations < n) {
                zImaginary = 2 * zReal * zImaginary + cImaginary;
                zReal = zReal2 - zImaginary2 + cReal;
                zReal2 = zReal * zReal;
                zImaginary2 = zImaginary * zImaginary;
                iterations++;
            }
            unsigned char *pixel = &img[(y_offset + x) * 4];

            if (iterations == n) {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel[3] = 255;
            } else {
                iterations = iterations % 16;
                pixel[0] = lookupTable1[iterations][0];
                pixel[1] = lookupTable1[iterations][1];
                pixel[2] = lookupTable1[iterations][2];
                pixel[3] = lookupTable1[iterations][3];
            }
            cReal = cReal + res;
        }
        cImaginary = cImaginary + res;
        y_offset = y_offset + width;
    }
}

