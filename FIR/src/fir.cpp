#include "../include/fir.h"

// Circular buffer FIR Filtering
void processSignal(float *inputDataChunk, float *outputDataChunk, float *firCoeffs, float *buffer,
                   int numSamples, int numFIRCoeffs, int bufferSize) {
    static int bufferIndex = 0;  

    // Process each sample in the input chunk
    for (int n = 0; n < numSamples; n++) {
        // Insert new sample in circular buffer
        buffer[bufferIndex] = inputDataChunk[n];

        // FIR filter cummulator
        float accum = 0.0;
        int index = bufferIndex;

        // Convolution sum
        // we go back in the buffer and increment the coefficients
        for (int k = 0; k < numFIRCoeffs; k++) {
            accum += firCoeffs[k] * buffer[index];
            index = (index - 1 + bufferSize) % bufferSize;  
        }

        outputDataChunk[n] = accum;

        // Move the buffer index 
        bufferIndex = (bufferIndex + 1) % bufferSize;
    }
}
