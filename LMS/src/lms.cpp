#include <math.h>

void processLMS(float *inputChunk, float *outputChunk, float *adaptiveCoeffs,
                          int numSamples, int numLMSCoeffs, float mu, float interfererFreq,
                          float sampleRate) {
    static float currentPhase = 0.0;  // Current phase of the sinusoidal reference
    float phaseIncrement = 2.0 * M_PI * interfererFreq / sampleRate;  // Phase increment per sample

    // Process each sample in the input chunk
    for (int n = 0; n < numSamples; n++) {
        // Compute LMS filter output (estimate of the interference)
        float filterOutput = 0.0;
        for (int k = 0; k < numLMSCoeffs; k++) {
            float pastReference = sinf(currentPhase - k * phaseIncrement);  // Reference for past samples
            filterOutput += adaptiveCoeffs[k] * pastReference;
        }

        // Compute error signal (desired signal minus estimated interference)
        float error = inputChunk[n] - filterOutput;

        // Save the error signal as the output
        outputChunk[n] = error;

        // Update adaptive coefficients using the LMS rule
        for (int k = 0; k < numLMSCoeffs; k++) {
            float pastReference = sinf(currentPhase - k * phaseIncrement);  
            adaptiveCoeffs[k] += 2 * mu * error * pastReference;
        }

        // Update the phase for the next sample
        currentPhase += phaseIncrement;
        if (currentPhase > 2.0 * M_PI) {
            currentPhase -= 2.0 * M_PI;  // Wrap phase to stay within [0, 2π]
        }
    }
}
