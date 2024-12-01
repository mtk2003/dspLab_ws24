#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Read input signal in chunks from CSV file
int read_chunk(FILE *file, double *buffer, int chunkSize);

// Write output signal in chunks to CSV file
void write_chunk(FILE *file, double *data, int dataSize);

// Read FIR coefficients from CSV
void readFIRCoeffsFromFile(FILE *fp, double *coeffs, size_t numFIRCoeffs);

#endif 