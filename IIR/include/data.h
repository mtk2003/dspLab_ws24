#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>

// IIR filter struct
typedef struct {
    double b0, b1, b2, a0, a1, a2;
    double x1, x2, y1, y2;
} Biquad;

// IIR filter coefficient and state variable init
void biquad_init(Biquad* filter, double b0, double b1, double b2, double a1, double a2, double inv_stage_gain);

// Read FIR coefficients from CSV. The values are stored as column vector.
void load_biquad_coefficients(const char* filename, Biquad* filters, size_t num_filters);

// Function to read a chunk of data from a CSV file
int read_chunk(FILE *file, double *buffer, int chunkSize);

// Function to write a chunk of processed data to a CSV file
void write_chunk(FILE *file, double *data, int dataSize);

#endif // DATA_H
