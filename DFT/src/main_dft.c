#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/complex.h"
#include "../include/data_utils.h"

// Define M_PI if not defined in math.h
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Function to compute DFT
void compute_dft(Complex *inputBuffer, Complex *outputBuffer, int NDFT) {
    for (int k = 0; k < NDFT; k++) {
        outputBuffer[k].real = 0;
        outputBuffer[k].imag = 0;
        for (int n = 0; n < NDFT; n++) {
            float phase = 2.0 * M_PI * k * n / NDFT;
            outputBuffer[k].real += inputBuffer[n].real * cos(phase) + inputBuffer[n].imag * sin(phase);
            outputBuffer[k].imag += inputBuffer[n].imag * cos(phase) - inputBuffer[n].real * sin(phase);
        }
    }
}

// Function to compute IDFT
void compute_idft(Complex *inputBuffer, Complex *outputBuffer, int NDFT) {
    for (int n = 0; n < NDFT; n++) {
        outputBuffer[n].real = 0;
        outputBuffer[n].imag = 0;
        for (int k = 0; k < NDFT; k++) {
            float phase = 2.0 * M_PI * k * n / NDFT;
            outputBuffer[n].real += inputBuffer[k].real * cos(phase) - inputBuffer[k].imag * sin(phase);
            outputBuffer[n].imag += inputBuffer[k].real * sin(phase) + inputBuffer[k].imag * cos(phase);
        }
        outputBuffer[n].real /= NDFT;
        outputBuffer[n].imag /= NDFT;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <number_of_samples> <input_file> <output_file> <dft|idft>\n", argv[0]);
        return 1;
    }

    int NDFT = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    char *mode = argv[4];

    // Allocate memory for input and output buffer
    Complex *inputBuffer = (Complex*) malloc(sizeof(Complex) * NDFT);
    Complex *outputBuffer = (Complex*) malloc(sizeof(Complex) * NDFT);

    if (!inputBuffer || !outputBuffer) {
        printf("Buffer allocation failed\n");
        return 1;
    }

    // Load real/complex signal from CSV
    if (!load_data_from_csv(input_file, inputBuffer, NDFT)) {
        return 1;
    }

    // Compute DFT/IDFT
    if (strcmp(mode, "dft") == 0) {
        compute_dft(inputBuffer, outputBuffer, NDFT);
    } else if (strcmp(mode, "idft") == 0) {
        compute_idft(inputBuffer, outputBuffer, NDFT);
    } else {
        printf("Invalid mode. Use 'dft' or 'idft'.\n");
        return 1;
    }

    // Save real/complex signal to CSV
    if (!save_data_to_csv(output_file, outputBuffer, NDFT)) {
        return 1;
    }

    free(inputBuffer);
    free(outputBuffer);

    printf("Results saved in %s\n", output_file);

    return 0;
}
