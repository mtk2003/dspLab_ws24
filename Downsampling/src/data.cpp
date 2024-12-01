#include "../include/data.h"

// Read data from CSV into a double array (real values only)
void readFIRCoeffsFromFile(FILE *fp, double *coeffs, size_t numFIRCoeffs) {
    for (size_t i = 0; i < numFIRCoeffs; i++) {
        if (fscanf(fp, "%lf\n", &coeffs[i]) != 1) {
            fprintf(stderr, "Error reading FIR coefficients at line %zu.\n", i + 1);
            fclose(fp); 
            return; 
        }
    }
    fclose(fp);
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
        fprintf(file, "%lf\n", data[i]);
    }
}