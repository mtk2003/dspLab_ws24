#include "../include/iir.h"

// Proess chunk of data
void process_chunk(double* input_chunk, double* output_chunk, size_t nSamples, Biquad* filters, size_t num_filters) {
    for (size_t i = 0; i < nSamples; ++i) {
        double sample = input_chunk[i];
        for (size_t j = 0; j < num_filters; ++j) {
            sample = biquad_process(&filters[j], sample);
        }
        output_chunk[i] = sample;
    }
}

// Single Biquad filter stage processing
double biquad_process(Biquad* filter, double in) {
    double out = filter->b0 * in + filter->b1 * filter->x1 + filter->b2 * filter->x2
                - filter->a1 * filter->y1 - filter->a2 * filter->y2;

    filter->x2 = filter->x1;
    filter->x1 = in;
    filter->y2 = filter->y1;
    filter->y1 = out;

    return out;
}
