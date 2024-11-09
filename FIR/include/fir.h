#ifndef FIR_H
#define FIR_H

#include <string.h>

void processSignal( float *input, float *output, float *firCoeffs, float *buffer,
       int nSamples, int numFIRCoeffs );

#endif