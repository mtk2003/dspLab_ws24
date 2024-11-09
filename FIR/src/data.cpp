#include "../include/data.h"

// Read data from CSV into a double array (real values only)
void readFIRCoeffsFromFile(const char *fileName, float *coeffs, size_t numFIRCoeffs) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file %s for reading.\n", fileName);
        return; 
    }

    for (size_t i = 0; i < numFIRCoeffs; i++) {
        if (fscanf(fp, "%f\n", &coeffs[i]) != 1) {
            fprintf(stderr, "Error reading from file %s at line %zu.\n", fileName, i + 1);
            fclose(fp); 
            return; 
        }
    }

    fclose(fp);
}
