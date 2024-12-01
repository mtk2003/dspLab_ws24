#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data.h"
#include "../include/ds.h"

void cleanup(double *coeffs, double *buffer, double *inputChunk, double *outputChunk, FILE *inputFile, FILE *outputFile);

// Cleanup function
void cleanup(double *coeffs, double *buffer, double *inputChunk, double *outputChunk, FILE *inputFile, FILE *outputFile) {
    if (coeffs) free(coeffs);
    if (buffer) free(buffer);
    if (inputChunk) free(inputChunk);
    if (outputChunk) free(outputChunk);
    if (inputFile) fclose(inputFile);
    if (outputFile) fclose(outputFile);
}

int main(int argc, char *argv[]) {
    if (argc != 8) {
        fprintf(stderr, "Usage: %s <input wav file> <output wav file> <FIR coeffs file> <num FIR coeffs> <chunk size> <downsampling factor> <Normalized mixinf frequency>\n", argv[0]);
        return 1;
    }

    // Read command line arguments
    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    char *firCoeffsFileName = argv[3];
    int numFIRCoeffs = atoi(argv[4]);
    int nSamplesPerChunk = atoi(argv[5]);
    int downSamplingFactor = atoi(argv[6]);
    double fmix = atof(argv[7]);

    // Validate arguments
    if (numFIRCoeffs <= 0 || nSamplesPerChunk <= 0 || downSamplingFactor <= 0 || fmix < 0) {
        fprintf(stderr, "Error: Invalid arguments. Ensure all values are positive.\n");
        return 1;
    }

    // Initialize file pointers for data loading
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

    FILE *firCoeffsFile = fopen(firCoeffsFileName, "r");
        if (firCoeffsFile == NULL) {
        fprintf(stderr, "Can't open output file!\n");
        fclose(inputFile);
        fclose(outputFile);
        return -1;
    }

    // Initialize data arrays
    int nSamplesPerOutputChunk = nSamplesPerChunk/downSamplingFactor;
    int bufferSize = nSamplesPerChunk+numFIRCoeffs-1;
    double *firCoeffs = (double*)malloc(numFIRCoeffs*sizeof(double));
    double *buffer = (double*)malloc(bufferSize*sizeof(double));
    double *inputChunk = (double*)malloc(nSamplesPerChunk*sizeof(double));
    double *outputChunk = (double*)malloc(nSamplesPerOutputChunk*sizeof(double));

    // Check memory allocation
    if (!firCoeffs || !buffer || !inputChunk || !outputChunk) {
        fprintf(stderr, "Failed to allocate memory\n");
        cleanup(firCoeffs, buffer, inputChunk, outputChunk, NULL, NULL);
        return -1;
    }

    // Initialize buffer to zero
    memset(buffer, 0, bufferSize * sizeof(double));
    memset(firCoeffs, 0, numFIRCoeffs * sizeof(double));
    memset(inputChunk, 0, nSamplesPerChunk * sizeof(double));
    memset(outputChunk, 0, nSamplesPerOutputChunk * sizeof(double));

    // Read FIR filter coefficients
    readFIRCoeffsFromFile(firCoeffsFile, firCoeffs, numFIRCoeffs);
    
    // Process signal in chunks
    // num_read is always <= nSamplesPerChunk
    int num_read = 0;
    int num_processed = 0;
    //int num_total = 0;
    while ((num_read = read_chunk(inputFile, inputChunk, nSamplesPerChunk)) > 0) {
        //num_total = num_total + num_read;
        //fprintf(stdout,"Samples processed %d\n", num_total);
        num_processed = processSignal( inputChunk, outputChunk, firCoeffs, buffer,
        num_read,numFIRCoeffs, bufferSize, downSamplingFactor, fmix);
        write_chunk(outputFile, outputChunk, num_processed);
    }

    // Free memory
    cleanup(firCoeffs, buffer, inputChunk, outputChunk, inputFile, outputFile);
    return 0;
}
