#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// read FIR coefficients from CSV. The values are stored as column vector.
void readFIRCoeffsFromFile(const char *fileName, float *coeffs, size_t numFIRCoeffs);

#endif 