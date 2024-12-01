#include "math.h"
#include "../include/ds.h"

// Circular buffer FIR Filtering
int processSignal(double *inputDataChunk, double *outputDataChunk, double *firCoeffs, double *buffer,
                   int numSamples, int numFIRCoeffs, int bufferSize, int downsamplingFactor, double normalizedFmix) {
    static int bufferIndex = 0;  
    static double phase = 0.0;
    int outputIndex = 0.0;
    double phaseIncrement = 2 * M_PI * normalizedFmix;
    double oscillator = 0.0;

    // Process each sample in the input chunk
    for (int n = 0; n < numSamples; n++) {

        // Calculate oscillator with current phase value for down-mixing
        oscillator = 2*cos(phase);

        // Insert new sample in circular buffer
        buffer[bufferIndex] = inputDataChunk[n] * oscillator;

        phase += phaseIncrement;
        if (phase >= 2.0 * M_PI)
        {
            phase -= 2.0 * M_PI;
        }
        
        // Downsampling
        if(n % downsamplingFactor == 0) {
            // FIR filter cummulator
            double accum = 0.0;
            int index = bufferIndex;

            // Convolution sum 
            // we go back in the buffer and increment the coefficients
            for (int k = 0; k < numFIRCoeffs; k++) {
                    accum += firCoeffs[k] * buffer[index];
                    index = (index - 1 + bufferSize) % bufferSize;  
            }

            // We store only every M-th sample
            outputDataChunk[outputIndex++] = accum * downsamplingFactor;
        }

        // Move the buffer index 
        bufferIndex = (bufferIndex + 1) % bufferSize;
    }

    return outputIndex;
}
