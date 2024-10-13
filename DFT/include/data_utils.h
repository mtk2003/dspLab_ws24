#ifndef DATA_UTILS_H
#define DATA_UTILS_H

#include <stdio.h>
#include "complex.h"

// Function to load complex data from a CSV file
int load_data_from_csv(const char *filename, Complex *data, int N) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    for (int i = 0; i < N; i++) {
        if (fscanf(file, "%f,%f", &data[i].real, &data[i].imag) != 2) {
            printf("Error reading data from file at index %d\n", i);
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

// Function to save complex data to a CSV file
int save_data_to_csv(const char *filename, Complex *data, int N) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return 0;
    }

    for (int i = 0; i < N; i++) {
        fprintf(file, "%.7f,%.7f\n", data[i].real, data[i].imag);  
    }

    fclose(file);
    return 1;
}

#endif 
