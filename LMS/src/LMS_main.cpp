#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>
#include <math.h>
#include "../include/lms.h"

// Cleanup function
void cleanup(float *adaptiveCoeffs, float *inputChunk, float *outputChunk, SNDFILE *infile, SNDFILE *outfile) {
    free(adaptiveCoeffs);
    free(inputChunk);
    free(outputChunk);
    if (infile) sf_close(infile);
    if (outfile) sf_close(outfile);
}

int main(int argc, char *argv[]) {
    // Expecting 7 arguments
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <input wav file> <output wav file> <num FIR coeffs> <buffer size> <learning rate (mu)> <interferer frequency>\n", argv[0]);
        return 1;
    }

    // Read command-line arguments
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int numLMSCoeffs = atoi(argv[3]);
    int nSamples = atoi(argv[4]);
    float mu = atof(argv[5]);
    float interfererFreq = atof(argv[6]);

    // Initialize data arrays
    float *adaptiveCoeffs = (float *)malloc(numLMSCoeffs * sizeof(float));
    float *inputChunk = (float *)malloc(nSamples * sizeof(float));
    float *outputChunk = (float *)malloc(nSamples * sizeof(float));
    SNDFILE *infile = NULL;
    SNDFILE *outfile = NULL;
    SF_INFO sfinfo;
    sf_count_t num_read;

    // Check memory allocation
    if (!adaptiveCoeffs || !inputChunk || !outputChunk) {
        fprintf(stderr, "Failed to allocate memory\n");
        cleanup(adaptiveCoeffs, inputChunk, outputChunk, NULL, NULL);
        return -1;
    }

    // Initialize arrays to zero
    memset(adaptiveCoeffs, 0, numLMSCoeffs * sizeof(float));  // Start with zero coefficients   
    memset(inputChunk, 0, nSamples * sizeof(float));
    memset(outputChunk, 0, nSamples * sizeof(float));

    // Open input WAV file
    infile = sf_open(inputFile, SFM_READ, &sfinfo);
    if (!infile) {
        fprintf(stderr, "Could not open input file: %s\n", inputFile);
        cleanup(adaptiveCoeffs, inputChunk, outputChunk, infile, NULL);
        return -1;
    }

    // Open output WAV file
    outfile = sf_open(outputFile, SFM_WRITE, &sfinfo);
    if (!outfile) {
        fprintf(stderr, "Could not open output file: %s\n", outputFile);
        cleanup(adaptiveCoeffs, inputChunk, outputChunk, infile, outfile);
        return -1;
    }

    // Process audio file in chunks
    while ((num_read = sf_read_float(infile, inputChunk, nSamples)) > 0) {
        processLMS(inputChunk, outputChunk, adaptiveCoeffs, num_read, numLMSCoeffs, mu, interfererFreq, sfinfo.samplerate);
        sf_write_float(outfile, outputChunk, num_read);
    }

    // Free resources
    cleanup(adaptiveCoeffs, inputChunk, outputChunk, infile, outfile);
    return 0;
}
