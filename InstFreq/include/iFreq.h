#ifndef INSTFREQ_H
#define INSTFREQ_H

#include <math.h>
#include "../include/data.h"

// ATAN2
void calculatePhases(complex *inputArray, float *wrappedPhase, size_t numElements);

// Phase unwrapping
void unwrapPhase(float *wrappedPhaseArray, float *unwrappedPhaseArray, size_t numElements, float *lastWrappedPhase, float *lastPhaseCorrection);

// Instantaneous frequency calculation
void calculateInstantaneousFrequency(float *unwrappedPhase, float *instFreq, int blockIdx, float *lastUnwrappedPhase, size_t numElements, float sampleRate);

#endif