#include "../include/data.h"
#include <stdio.h>
#include <stdlib.h>

// Initi function for the biquad structure and gain application
void biquad_init(Biquad* filter, double b0, double b1, double b2, double a1, double a2, double inv_stage_gain) {
    filter->b0 = b0 * inv_stage_gain;
    filter->b1 = b1 * inv_stage_gain;
    filter->b2 = b2 * inv_stage_gain;
    filter->a1 = a1;
    filter->a2 = a2;
    filter->x1 = 0.0;
    filter->x2 = 0.0;
    filter->y1 = 0.0;
    filter->y2 = 0.0;

    printf("Initialized filter: b0=%lf, b1=%lf, b2=%lf, a1=%lf, a2=%lf\n",
        filter->b0, filter->b1, filter->b2, filter->a1, filter->a2);
}

// Load IIR filter coefficients.
void load_biquad_coefficients(const char* filename, Biquad* filters, size_t num_filters) {
    printf("Loading biquad coefficients from %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open coefficient file");
        exit(EXIT_FAILURE);
    }

    double inv_stage_gain, b0, b1, b2, a0, a1, a2;
    size_t filter_idx = 0;

    while (filter_idx < num_filters && fscanf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", &inv_stage_gain, &b0, &b1, &b2, &a0, &a1, &a2) == 7) {
        printf("Parsed coefficients (skipping a0): inv_stage_gain=%lf, b0=%lf, b1=%lf, b2=%lf, a1=%lf, a2=%lf\n", inv_stage_gain, b0, b1, b2, a1, a2);
        biquad_init(&filters[filter_idx], b0, b1, b2, a1, a2, inv_stage_gain);
        filter_idx++;
    }

    if (filter_idx != num_filters) {
        fprintf(stderr, "Error reading coefficients. Only %zu filters were loaded from file %s\n", filter_idx, filename);
    }

    fclose(file);
    printf("Successfully loaded %zu biquad filters.\n", filter_idx);
}

// Read a chunk of input data
int read_chunk(FILE *file, double *buffer, int chunkSize) {
    int count = 0;
    while (count < chunkSize && fscanf(file, "%lf", &buffer[count]) == 1) {
        count++;
    }
    return count;
}

// Write a chunk of output data
void write_chunk(FILE *file, double *data, int dataSize) {
    for (int i = 0; i < dataSize; i++) {
        fprintf(file, "%.2f\n", data[i]);
    }
}
