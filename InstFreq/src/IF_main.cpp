#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data.h"
#include "../include/iFreq.h"
#include <assert.h>

// Function prototypes
void cleanup(complex *inputChunk, float *outputChunk, float *wrappedPhase, float *unWrappedPhase, float* instFreq);

// Cleanup resources and handle memory management
void cleanup(complex *inputChunk, float *outputChunk, float *wrappedPhase, float *unWrappedPhase, float* instFreq) {
    if(inputChunk) free(inputChunk);
    if(outputChunk) free(outputChunk);
    if(wrappedPhase) free(wrappedPhase);
    if(unWrappedPhase) free(unWrappedPhase);
    if(instFreq) free(instFreq);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input csv file> <output csv file> <chunkSize> <sampleRate>\n", argv[0]);
        return 1;
    }

    // Read command line arguments
    char *inputFileName = argv[1];
    char *outputFileName = argv[2];
    int chunkSize = atoi(argv[3]);
    float sampleRate = atof(argv[4]);

    // Initialize data arrays
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {
        fprintf(stderr, "Can't open input file!\n");
        return -1;
    }
    
    FILE *outputFile = fopen(outputFileName, "w");
        if (outputFile == NULL)
    {
        fprintf(stderr, "Can't open output file!\n");
        return -1;
    }

    complex *inputChunk = (complex*)malloc(chunkSize * sizeof(complex));
    float *outputChunk = (float*)malloc((chunkSize-1) * sizeof(float));
    float *wrappedPhase = (float*)malloc(chunkSize * sizeof(float));
    float *unWrappedPhase = (float*)malloc(chunkSize * sizeof(float));
    float *instFreq = (float*)malloc(chunkSize * sizeof(float));

    // Check memory allocation
    if (!inputChunk || !outputChunk || !wrappedPhase || !unWrappedPhase || !instFreq ) {
        fprintf(stderr, "Failed to allocate memory\n");
        cleanup(inputChunk, outputChunk,  wrappedPhase, unWrappedPhase, instFreq);
        return -1;
    }

    // Initialize buffer to zero
    memset(inputChunk, 0, chunkSize * sizeof(float));
    memset(outputChunk, 0, (chunkSize-1) * sizeof(float));
    memset(wrappedPhase, 0, chunkSize * sizeof(float));
    memset(unWrappedPhase, 0, chunkSize * sizeof(float));
    memset(instFreq, 0, (chunkSize) * sizeof(float));

    int num_read = 0;

    // Process data in chunks simulating block data reception
    float lastWrappedPhase = 0.0;
    float lastPhaseCorrection = 0.0;
    float lastUnwrappedPhase = 0.0;
    int counter = 0;
    while ((num_read = read_chunk(inputFile, inputChunk, chunkSize)) > 0) {  

        printf("Processing chunk %d\n", counter);

        calculatePhases(inputChunk, wrappedPhase, num_read);
        unwrapPhase(wrappedPhase, unWrappedPhase, num_read, &lastWrappedPhase, &lastPhaseCorrection);
        calculateInstantaneousFrequency(unWrappedPhase, instFreq, counter, &lastUnwrappedPhase, num_read, sampleRate);
        
        lastWrappedPhase = wrappedPhase[num_read-1];
        lastUnwrappedPhase = unWrappedPhase[num_read-1];

        write_chunk(outputFile, instFreq, (counter==0 ? num_read-1 : num_read));  
        
        counter++;
    }

    // Free memory
    cleanup(inputChunk, outputChunk, wrappedPhase, unWrappedPhase, instFreq);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
