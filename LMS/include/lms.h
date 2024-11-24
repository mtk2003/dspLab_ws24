#ifndef LMS_H
#define LMS_H

#include <string.h>

void processLMS(float *inputChunk, float *outputChunk, float *adaptiveCoeffs,
                          int numSamples, int numLMSCoeffs, float mu, float interfererFreq,
                          float sampleRate);

#endif