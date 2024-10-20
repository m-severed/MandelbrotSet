//
// Created by Philip Tran on 09.07.2023.
//

#include <complex.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "default_image_performance.h"
#include "simd_image_performance.h"
#include "optimized_sisd_performance_image.h"


// Messages to be displayed if the user set the help option
const char *usage_examples =
        "Usage examples:\n"
        "  performance_test -s -2.0 -1.5 -d 1200 800 -n 5000 -r 0.00625\n";

const char *help_msg =
        "Usage: mandelbrot [OPTIONS]\n\n"
        "Optional flags:\n"
        "  -B n     Measure and output the runtime of all implementations. The argument specifies the number of trials.\n"
        "  -s x,y   Set the starting point of the calculation. The default values are x = -2.0 and y = -1.0.\n"
        "  -d w,h   Set the width and height of the image. The default is w = 1920 and h = 1920.\n"
        "  -n N     Set the maximum number of iterations. The default is N = 10000.\n"
        "  -r res   Set the step size per pixel. The default is res = 0.000625.\n"
        "  -h       Show this help message and exit.\n";

void print_usage_examples() {
    fprintf(stderr, "%s", usage_examples);
}

void print_help() {
    print_usage_examples();
    fprintf(stderr, "\n%s", help_msg);
}

void strategy_implementation(float complex start, int32_t width, int32_t height, float res, unsigned n, unsigned char *img,
                        unsigned imp) {
    switch (imp) {
        case 0:
            default_mandelbrot_image(start, width, height, res, n, img);
            break;
        case 1:
            mandelbrot_optimized_image(start, width, height, res, n, img);
            break;
        case 2:
            mandelbrot_simd_optimization_new_image(start, width, height, res, n, img);
            break;
        default:
            break;
    }
}


int main(int argc, char *argv[]) {
    int opt;
    // different tests that are run
    char arr[3][200] = {"Naive implementation.",
                        "Optimized SISD implementation.",
                        "SIMD implementation."
    };

    // default values that the test will run with unless the user provide other
    int repetitions = 100;
    float x = -2.0f;
    float y = -1.0f;
    int32_t width = 1920;
    int32_t height = 1920;
    float stepSize = 0.000625f;
    unsigned maxIterations = 10000;
    unsigned char *img;
    double elapsedTime;

    // use getopt to read and parse any arguments that are given by the user
    while ((opt = getopt(argc, argv, "B:s:d:n:r:h")) != -1) {
        switch (opt) {
            case 'B':
                repetitions = atoi(optarg);
                break;
            case 's':
                x = atof(optarg);
                y = atof(argv[optind]);
                optind++;
                break;
            case 'd':
                width = atoi(optarg);
                height = atoi(argv[optind]);
                optind++;
                break;
            case 'n':
                maxIterations = atoi(optarg);
                break;
            case 'r':
                stepSize = atof(optarg);
                break;
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            default:
                printf("Implement this feature later\n");
                return EXIT_FAILURE;
        }
    }

    printf("Performing Mandelbrot calculation with %d trials\n", repetitions);
    printf("Start point: %f, %f\n", x, y);
    printf("Image size: %d, %d\n", width, height);
    printf("Max iterations: %d\n", maxIterations);
    printf("Step size: %.8f\n", stepSize);

    // get the length of the array with tests
    int numStrings = sizeof(arr) / sizeof(arr[0]);

    // perform time measurements on the given set of tests
    for (int i = 0; i < numStrings; ++i) {
        int temp = repetitions;
        printf("%s\n", arr[i]);
        // allocate at least width * height * 4 bytes in order to satisfy all possible implementations, allocating more space than needed shouldn't affect the total time
        img = (unsigned char *) malloc(width * height * 4);
        if (img == NULL) {
            printf("Memory allocation failed.\n");
            return EXIT_FAILURE;
        }
        struct timespec start_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        while (temp-- > 0) {
            strategy_implementation(x + y * I, width, height, stepSize, maxIterations, img, i);
        }
        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        elapsedTime = end_time.tv_sec - start_time.tv_sec + 1e-9 * (end_time.tv_nsec - start_time.tv_nsec);

        double averageTime = elapsedTime / repetitions;
        free(img);
        printf("Average execution time: %f seconds\n", averageTime);
    }

    return EXIT_SUCCESS;
}
