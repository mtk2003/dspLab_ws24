#include "../include/data.h"

// Function to read a chunk of data from a CSV file, file should be opened before calling this function
int read_chunk(FILE *file, complex *buffer, int chunkSize) {
    int count = 0;
    while (count < chunkSize) {
        // Assuming the real and imaginary parts are comma-separated
        if (fscanf(file, "%f,%f\n", &buffer[count].real, &buffer[count].imag) == 2) {
            count++;
        } else {
            break; // Break the loop if the data format does not match or end of file is reached
        }
    }
    return count; // Returns the number of complex numbers actually read
}

// Function to write a chunk of processed data to a CSV file, file should be opened before calling this function
void write_chunk(FILE *file, float *data, int dataSize) {
    for (int i = 0; i < dataSize; i++) {
        fprintf(file, "%f\n", data[i]);  // Each data sample is written to a new line
    }
}
