#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data.h"
#include "../include/iir.h"

// Function prototype for cleanup
void cleanup(Biquad *coeffs, double *inputChunk, double *outputChunk, FILE *inputFile, FILE *outputFile);

// Cleanup resources and handle memory management
void cleanup(Biquad *coeffs, double *inputChunk, double *outputChunk, FILE *inputFile, FILE *outputFile) {
    free(coeffs);
    free(inputChunk);
    free(outputChunk);
    if (inputFile) fclose(inputFile);
    if (outputFile) fclose(outputFile);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <input csv file> <output csv file> <IIR coeffs file> <num IIR filters> <buffer size>\n", argv[0]);
        return 1;
    }

    // Read command line arguments
    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    char *iirCoeffsFile = argv[3];
    int numIIRFilters = atoi(argv[4]);
    int nChunk = atoi(argv[5]);

    // Initialize data arrays
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Can't open input file!\n");
        return -1;
    }
    
    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Can't open output file!\n");
        fclose(inputFile);
        return -1;
    }

    Biquad *iirCoeffs = (Biquad*)malloc(numIIRFilters * sizeof(Biquad));
    double *inputChunk = (double*)malloc(nChunk * sizeof(double));
    double *outputChunk = (double*)malloc(nChunk * sizeof(double));

    // Check memory allocation
    if (!iirCoeffs || !inputChunk || !outputChunk) {
        fprintf(stderr, "Failed to allocate memory\n");
        cleanup(iirCoeffs, inputChunk, outputChunk, inputFile, outputFile);
        return -1;
    }

    // Initialize buffer to zero
    memset(iirCoeffs, 0, numIIRFilters * sizeof(Biquad));
    memset(inputChunk, 0, nChunk * sizeof(double));
    memset(outputChunk, 0, nChunk * sizeof(double));

    // Read IIR filter coefficients
    load_biquad_coefficients(iirCoeffsFile, iirCoeffs, numIIRFilters);

    int num_read = 0;
    // Process audio file in chunks
    while ((num_read = read_chunk(inputFile, inputChunk, nChunk)) > 0) {
        process_chunk(inputChunk, outputChunk, num_read, iirCoeffs, numIIRFilters);
        write_chunk(outputFile, outputChunk, num_read);
    }

    // Free memory and close files
    cleanup(iirCoeffs, inputChunk, outputChunk, inputFile, outputFile);
    return 0;
}
