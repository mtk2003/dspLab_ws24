#ifndef DS_H
#define DS_H

#include <string.h>

int processSignal( double *input, double *output, double *firCoeffs, double *buffer,
       int nSamples, int numFIRCoeffs, int bufferSize, int downsamplingFactor, double fmix);

#endif