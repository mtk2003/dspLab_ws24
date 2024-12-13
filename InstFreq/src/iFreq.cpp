#include "../include/iFreq.h"

void calculatePhases(complex *inputArray, float *wrappedPhase, size_t numElements) {
    for (size_t i = 0; i < numElements; i++) {
        wrappedPhase[i] = atan2(inputArray[i].imag, inputArray[i].real);
    }
}

void unwrapPhase(float *wrappedPhaseArray, float *unwrappedPhaseArray, size_t numElements, float *lastWrappedPhase, float *lastPhaseCorrection) {
    double K = *lastPhaseCorrection;
    double difference = wrappedPhaseArray[0] - *lastWrappedPhase;
    
    if (difference > M_PI) {
        K -= 2 * M_PI;
    } else if (difference < -M_PI) {
        K += 2 * M_PI;
    }

    unwrappedPhaseArray[0] = wrappedPhaseArray[0] + K;

    for (int i = 1; i < numElements; i++) {
        difference = wrappedPhaseArray[i] - wrappedPhaseArray[i - 1];
        
        if (difference > M_PI) {
            K -= 2 * M_PI;
        } else if (difference < -M_PI) {
            K += 2 * M_PI;
        }
        
        unwrappedPhaseArray[i] = wrappedPhaseArray[i] + K;
    }
    
    *lastPhaseCorrection = K;
}

void calculateInstantaneousFrequency(float *unwrappedPhase, float *instFreq, int blockIdx, float *lastUnwrappedPhase, size_t numElements, float sampleRate) {
    if (blockIdx == 0) {
        for (size_t idx = 1; idx < numElements; idx++) {
            instFreq[idx - 1] = (unwrappedPhase[idx] - unwrappedPhase[idx - 1]) * sampleRate / (2 * M_PI);
        }
    } else {
        instFreq[0] = (unwrappedPhase[0] - *lastUnwrappedPhase) * sampleRate / (2 * M_PI);
        for (size_t idx = 1; idx < numElements; idx++) {
            instFreq[idx] = (unwrappedPhase[idx] - unwrappedPhase[idx - 1]) * sampleRate / (2 * M_PI);
        }
    }
}


