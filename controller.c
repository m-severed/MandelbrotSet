//
// Created by Philip Tran on 25.06.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <string.h>
#include <time.h>
#include "image_generator/image_generator.h"
#include "image_generator/image_generator_4_pixels.h"
#include "strategy/implementation_strategy.h"


// Messages to be displayed if the user set the help option
// example of how to use the option flags
const char *usage_examples =
        "Usage examples:\n"
        "  mandelbrot -s -2.0 -1.5 -d 1200 800 -n 5000 -o output\n"
        "  mandelbrot -B 10\n";

// Displays all the possible flags and what argument could be passed (In case the user doesn't pass a certain argument, show what is the default value
const char *help_msg =
        "Usage: mandelbrot [OPTIONS]\n\n"
        "Optional arguments:\n"
        "  -V n     Choose which implementation should be used to generate the Mandelbrot Set. -V 0 chooses the main implementation.\n"
        "           If the argument is not provided, the program will use by default the main implementation.\n"
        "  -B n     Measure and output the runtime of the specified implementation. The optional argument specifies the number of repetitions.\n"
        "  -s x,y   Set the starting point of the calculation. The default is x = -2.0 and y = -1.0.\n"
        "  -g x,y   Set the point to zoom in to. The default is x = -0.75 and y = 0.1\n"
        "  -d w,h   Set the width and height of the image. The default is w = 4800 and h = 3200.\n"
        "  -n N     Set the maximum number of iterations per pixel. The default is N = 2000.\n"
        "  -r res   Set the step size per pixel. The default is res = 0.000625.\n"
        "  -o file  Set the output file name. The default is 'mandelbrot.bmp'.\n"
        "  -t n     Generate a batch of N images with slightly different values, doesn't perform the measurement.\n"
        "  -Z z     Set the zoom factor, default zoom factor is 0.7.\n"
        "  -h       Show this help message and exit.\n"
        "  --help   Show this help message and exit.\n\n";

void print_usage_examples() {
    fprintf(stderr, "%s", usage_examples);
}

void print_help() {
    print_usage_examples();
    fprintf(stderr, "\n%s", help_msg);
}

// Helper method to concat two strings
char *concat(const char *s1, const char *s2) {
    // allocate appropriate space
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // null check if the malloc was successful otherwise terminate
    if (result == NULL) {
        printf("Failed to allocate memory for the destination.\n");
        return NULL;
    }

    // concatenate the strings
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


// Helper function to display the variables the program is currently working with
void performMandelbrot(int implementation, int repetitions, double startX, double startY, double destX, double destY,
                       int width, int height, int maxIterations, double stepSize, const char *outputFile) {
    printf("Performing Mandelbrot calculation...\n");
    printf("Implementation: %d\n", implementation);
    printf("Repetitions: %d\n", repetitions);
    printf("Start point: %.2f, %.2f\n", startX, startY);
    printf("End point: %.2f, %.2f\n", destX, destY);
    printf("Image size: %d x %d\n", width, height);
    printf("Max iterations: %d\n", maxIterations);
    printf("Step size: %.15f\n", stepSize);
    printf("Output file: %s\n", outputFile);
}

// Helper method to generate batch of images
void generateBatch(char *destination, double startX, double startY, double destX, double destY, int width, int height,
                   int maxIterations, double stepSize, int implementation) {
    // define the pixel size based on the implementation - SIMD implementation uses 4 byte pixels
    int32_t pixel_size = implementation == 0 ? 4 : 3;
    unsigned char *img = (unsigned char *) malloc(width * height * pixel_size);

    if (img == NULL) {
        printf("Failed to allocate memory for the image.\n");
        return;
    }

    FILE *file = fopen(destination, "wb");

    if (file == NULL) {
        printf("Failed to create the BMP file.\n");
        free(img);
        return;
    }


    performMandelbrot(implementation, 0, startX, startY, destX, destY, width, height, maxIterations, stepSize,
                      destination);
    mandelbrotStrategy(startX + startY * I, width, height, stepSize, maxIterations, img, implementation);

    if(implementation == 0) {
        BMP_4P(destination, img, width, height);
    } else {
        BMP(destination, img, width, height);
    }

    free(img);

}


int main(int argc, char *argv[]) {
    int opt;

    //Default settings if no options are provided
    int implementation = 0;
    int repetitions = -1;
    float startX = -2;
    float startY = -1;
    float endX;
    float destX = -0.75f;
    float destY = 0.1f;
    float zoomFactor = 0.7f;
    int32_t width = 4200;
    int32_t height = 3800;
    unsigned maxIterations = 2000;
    unsigned batch = 0;
    float stepSize = 0.000625f;
    char *outputDirectory = "results/";
    char *outputFile = "mandelbrot";
    char *fileType = ".bmp";
    int firstIteration = -1;

    // check if --help flag was used by the user, if so display the help message and terminate the program with EXIT_SUCCESS
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return EXIT_SUCCESS;;
        }
    }

    int tempCheck;
    // use getopt to read and parse the passed arguments
    while ((opt = getopt(argc, argv, "V:B:s:g:d:n:r:o:t:Z:h")) != -1) {
        switch (opt) {
            case 'V':
                implementation = atoi(optarg);
                break;
            case 'B':
                repetitions = atoi(optarg);
                break;
            case 's':
                startX = atof(optarg);
                startY = atof(argv[optind]);
                optind++;
                break;
            case 'g':
                destX = atof(optarg);
                destY = atof(argv[optind]);
                optind++;
                break;
            case 'd':
                tempCheck = atoi(optarg);
                if(tempCheck < 0) {
                    printf("Invalid arguments, please enter correct ones.\n");
                    return EXIT_FAILURE;
                }
                width = tempCheck;

                tempCheck = atoi(argv[optind]);
                if(tempCheck < 0) {
                    printf("Invalid arguments, please enter correct ones.\n");
                    return EXIT_FAILURE;
                }
                height = tempCheck;
                optind++;
                break;
            case 'n':
                tempCheck = atoi(optarg);
                if(tempCheck < 0) {
                    printf("Invalid arguments, please enter correct ones.\n");
                    return EXIT_FAILURE;
                }
                maxIterations = tempCheck;
                break;
            case 'r':
                stepSize = atof(optarg);
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 't':
                tempCheck = atoi(optarg);
                if(tempCheck < 0) {
                    printf("Invalid arguments, please enter correct ones.\n");
                    return EXIT_FAILURE;
                }
                batch = tempCheck;
                break;
            case 'Z':
                zoomFactor = atof(optarg);
                break;
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            default:
                printf("Implement this feature later\n");
                return EXIT_FAILURE;
        }
    }


    if(zoomFactor < 0 || strcmp(outputFile, "") == 0 || implementation < 0 || implementation > 2 || strchr(outputFile, '.') != NULL) {
        printf("Invalid arguments, please enter correct ones.\n");
        return EXIT_FAILURE;
    }

    // if the batch is set, generate n images and terminate the program afterward
    if (batch > 0) {
        unsigned count = 1;
        while (batch >= count) {
            // generate the name of the destination
            char *temp = concat(outputDirectory, outputFile);
            char num[10];
            snprintf(num, sizeof(num), "%d", count);
            char *appendNumber = concat(temp, num);
            char *destination = concat(appendNumber, fileType);

            generateBatch(destination, startX, startY, destX, destY, width, height, maxIterations, stepSize,
                          implementation);

            if (firstIteration < 0) {
                firstIteration++;
                startX = destX - 0.5f;
                startY = destY - 0.5f;
                endX = destX + 0.5f;
                stepSize = (endX - startX) / (float) width;
            } else {
                startX = (startX - destX) * zoomFactor + destX;
                startY = (startY - destY) * zoomFactor + destY;
                endX = (endX - destX) * zoomFactor + destX;
                stepSize = (endX - startX) / (float) width;
            }

            count++;
        }
        return EXIT_SUCCESS;
    }

    // generate the destination path
    char *temp = concat(outputDirectory, outputFile);
    char *destination = concat(temp, fileType);

    performMandelbrot(implementation, repetitions, startX, startY, destX, destY, width, height, maxIterations, stepSize,
                      destination);

    // define the pixel size based on the implementation - SIMD implementation uses 4 byte pixels
    int32_t pixel_size = implementation == 0 ? 4 : 3;
    unsigned char *img = (unsigned char *) malloc(width * height * pixel_size);

    // null check if the malloc was successful or not, in case of it being unsuccessful terminate
    if (img == NULL) {
        printf("Failed to allocate memory for the image.\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(destination, "wb");

    if (file == NULL) {
        printf("Failed to create the BMP file.\n");
        free(img);
        return EXIT_FAILURE;
    }

    // in case -B flag was used, perform measurements with termination afterward (no images created)
    if (repetitions > 0) {
        struct timespec start_time;
        printf("Measuring for %d repetitions\n", repetitions);
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        while (repetitions-- > 0) {
            mandelbrotStrategy(startX + startY * I, width, height, stepSize, maxIterations, img, implementation);
        }
        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        double elapsedTime = end_time.tv_sec - start_time.tv_sec + 1e-9 * (end_time.tv_nsec - start_time.tv_nsec);
        printf("Measurement done after %f seconds\n", elapsedTime);
        return EXIT_SUCCESS;
    } else {
        // use strategy pattern to choose the correct implementation
        mandelbrotStrategy(startX + startY * I, width, height, stepSize, maxIterations, img, implementation);
    }

    if(implementation == 0) {
        BMP_4P(destination, img, width, height);
    } else {
        BMP(destination, img, width, height);
    }


    free(img);

    return EXIT_SUCCESS;
}
