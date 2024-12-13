#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Complex data struct
typedef struct data
{
    float real;
    float imag;
} complex;

// Function to read a chunk of data from a CSV file
int read_chunk(FILE *file, complex *buffer, int chunkSize);

// Function to write a chunk of processed data to a CSV file
void write_chunk(FILE *file, float *data, int dataSize);

#endif 