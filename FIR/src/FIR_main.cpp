#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>
#include "../include/data.h"
#include "../include/fir.h"


void cleanup(float *coeffs, float *buffer, float *inputChunk, float *outputChunk, SNDFILE *infile, SNDFILE *outfile);

// Cleanup 
void cleanup(float *coeffs, float *buffer, float *inputChunk, float *outputChunk, SNDFILE *infile, SNDFILE *outfile) {
    free(coeffs);
    free(buffer);
    free(inputChunk);
    free(outputChunk);
    sf_close(infile);
    sf_close(outfile);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <input wav file> <output wav file> <FIR coeffs file> <num FIR coeffs> <buffer size>\n", argv[0]);
        return 1;
    }

    // Read command line arguments
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    char *firCoeffsFile = argv[3];
    int numFIRCoeffs = atoi(argv[4]);
    int nSamples = atoi(argv[5]);

    // Initialize data arrays
    int bufferSize = nSamples+numFIRCoeffs-1;
    float *firCoeffs = (float*)malloc(numFIRCoeffs*sizeof(float));
    float *buffer = (float*)malloc(bufferSize*sizeof(float));
    float *inputChunk = (float*)malloc(nSamples*sizeof(float));
    float *outputChunk = (float*)malloc(nSamples*sizeof(float));
    SNDFILE *infile = NULL; 
    SNDFILE *outfile = NULL;
    SF_INFO sfinfo;
    sf_count_t num_read;

    // Check memory allocation
    if (!firCoeffs || !buffer || !inputChunk || !outputChunk) {
        fprintf(stderr, "Failed to allocate memory\n");
        cleanup(firCoeffs, buffer, inputChunk, outputChunk, NULL, NULL);
        return -1;
    }

    // Initialize buffer to zero
    memset(buffer, 0, bufferSize * sizeof(float));
    memset(firCoeffs, 0, numFIRCoeffs * sizeof(float));
    memset(inputChunk, 0, nSamples * sizeof(float));
    memset(outputChunk, 0, nSamples * sizeof(float));

    // Read FIR filter coefficients
    readFIRCoeffsFromFile(firCoeffsFile, firCoeffs, numFIRCoeffs);
    
    // Input WAV file pointer
    infile = sf_open(inputFile, SFM_READ, &sfinfo);
    if (!infile) {
        fprintf(stderr, "Could not open input file: %s\n", inputFile);
        cleanup(firCoeffs, buffer, inputChunk, outputChunk, infile, NULL);  // Pass infile which is NULL
        return -1;
    }

    // Output WAV file pointer
    outfile = sf_open(outputFile, SFM_WRITE, &sfinfo);
    if (!outfile) {
        fprintf(stderr, "Could not open output file: %s\n", outputFile);
        cleanup(firCoeffs, buffer, inputChunk, outputChunk, infile, outfile); // outfile is NULL here
        return -1;
    }

    // Process audio file in chunks
    while ((num_read = sf_read_float(infile, inputChunk, nSamples)) > 0) {
        processSignal( inputChunk, outputChunk, firCoeffs, buffer,
        num_read,numFIRCoeffs);
        sf_write_float(outfile, outputChunk, num_read);
    }

    // Free memory
    cleanup(firCoeffs, buffer, inputChunk, outputChunk, infile, outfile);
    return 0;
}
