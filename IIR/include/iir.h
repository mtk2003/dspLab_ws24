#ifndef IIR_H
#define IIR_H

#include "../include/data.h"

// Process the input data in chunks
void process_chunk(double* input_chunk, double* output_chunk, size_t size, Biquad* filters, size_t num_filters);

// Process each chunk with serially-cascaded IIR filters
double biquad_process(Biquad* filter, double in);

#endif // IIR_H
