#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/complex.h"
#include "../include/data_utils.h"

// Define M_PI if not defined in math.h
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// K-means clustering
void compute_clusters(float *data, int NDFT, int numClusters, int maxIterations, int *labels, float *centroids) {
    int changes;  // Track number of changes in each iteration
    float minDist, dist;  // Distances for finding the nearest centroid
    int labelMatch;  // Stores the closest centroid for a data point

    // Arrays to store sum and count of points in each cluster for centroid update
    float sumCentroids[numClusters];
    int numPointsPerCentroid[numClusters];

    // Step 1: Initialize centroids with the first `numClusters` points
    for (size_t j = 0; j < numClusters; j++) {
        centroids[j] = data[j];
    }

    // Main loop: Iterate up to `maxIterations` times
    for (size_t k = 0; k < maxIterations; k++) {
        changes = 0;  // Reset changes counter

        // Step 2: Assign each data point to the nearest centroid
        for (size_t i = 0; i < NDFT; i++) {
            minDist = fabs(data[i] - centroids[0]);  // Distance to the first centroid
            labelMatch = 0;  // Start with the first centroid as the closest

            // Check each centroid to find the nearest one
            for (size_t j = 1; j < numClusters; j++) {
                dist = fabs(data[i] - centroids[j]);
                if (dist < minDist) {
                    minDist = dist;
                    labelMatch = j;
                }
            }

            // Update label if the nearest centroid has changed
            if (labels[i] != labelMatch) {
                labels[i] = labelMatch;
                changes++;  // Increment changes count
            }
        }

        // Step 3: Check for convergence
        if (changes == 0) {
            break;  // Stop if no points changed clusters
        } else {
            // Reset sums and counts for recalculating centroids
            memset(sumCentroids, 0, sizeof(sumCentroids));
            memset(numPointsPerCentroid, 0, sizeof(numPointsPerCentroid));

            // Accumulate data points for each centroid
            for (size_t i = 0; i < NDFT; i++) {
                sumCentroids[labels[i]] += data[i];  // Sum for the assigned cluster
                numPointsPerCentroid[labels[i]]++;  // Count points in each cluster
            }

            // Step 4: Update centroids as the mean of assigned points
            for (size_t j = 0; j < numClusters; j++) {
                if (numPointsPerCentroid[j] > 0) {  // Avoid division by zero
                    centroids[j] = sumCentroids[j] / numPointsPerCentroid[j];
                }
            }
        }
    }
}

// Function to compute DFT
void compute_dft(Complex *inputBuffer, Complex *outputBuffer, int NDFT) {
    for (int k = 0; k < NDFT; k++) {
        outputBuffer[k].real = 0;
        outputBuffer[k].imag = 0;
        for (int n = 0; n < NDFT; n++) {
            float phase = 2.0 * M_PI * k * n / NDFT;
            outputBuffer[k].real += inputBuffer[n].real * cos(phase) + inputBuffer[n].imag * sin(phase);
            outputBuffer[k].imag += inputBuffer[n].imag * cos(phase) - inputBuffer[n].real * sin(phase);
        }
    }
}

// Function to compute IDFT
void compute_idft(Complex *inputBuffer, Complex *outputBuffer, int NDFT) {
    for (int n = 0; n < NDFT; n++) {
        outputBuffer[n].real = 0;
        outputBuffer[n].imag = 0;
        for (int k = 0; k < NDFT; k++) {
            float phase = 2.0 * M_PI * k * n / NDFT;
            outputBuffer[n].real += inputBuffer[k].real * cos(phase) - inputBuffer[k].imag * sin(phase);
            outputBuffer[n].imag += inputBuffer[k].real * sin(phase) + inputBuffer[k].imag * cos(phase);
        }
        outputBuffer[n].real /= NDFT;
        outputBuffer[n].imag /= NDFT;
    }
}

// Compute the squared magnitude of the DFT output which is the power
void compute_squared_magnitude(Complex *inputDFT, float *squared_magnitude, int NDFT) {
    for (int i = 0; i < NDFT; i++) {
        squared_magnitude[i] = inputDFT[i].real * inputDFT[i].real + inputDFT[i].imag * inputDFT[i].imag;
    }
}

// Compute the logarithmic amplitude
void compute_magnitude_dB(Complex *inputDFT, float * magnitude_dB, int NDFT){
    float magnitudeLinear = 0.0;
    for (int i = 0; i < NDFT; i++)
    {
        magnitudeLinear = sqrt(inputDFT[i].real*inputDFT[i].real+inputDFT[i].imag*inputDFT[i].imag);
        magnitude_dB[i] = 20*log10(magnitudeLinear);
    }    
}

// Cell Averaging CFAR
void apply_cfar(float *squared_magnitude, float *cfar_threshold, int NDFT, int N_ref, int N_guard, float threshold_factor) {
    float alpha = -logf(threshold_factor);
    for (int i = 0; i < NDFT; i++) {

        int num_ref_cells = 0;
        float sum_ref_cells = 0.0;

        // Left reference cells
        for (int j = i - N_guard - N_ref; j < i - N_guard; j++) {
            if (j >= 0) {
                sum_ref_cells += squared_magnitude[j];
                num_ref_cells++;
            }
        }

        // Right reference cells
        for (int j = i + N_guard + 1; j <= i + N_guard + N_ref; j++) {
            if (j < NDFT) {
                sum_ref_cells += squared_magnitude[j];
                num_ref_cells++;
            }
        }

        // CFAR threshold calculation
        if (num_ref_cells > 0) {
            float avg_noise_power = sum_ref_cells / num_ref_cells;
            cfar_threshold[i] = avg_noise_power * alpha;  // Apply the alpha factor
        } else {
            cfar_threshold[i] = 0.0;  // No valid reference cells, set threshold to 0
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <number_of_samples> <input_file> <output_file> <dft|idft|cfar> [threshold_factor N_ref N_guard]\n", argv[0]);
        return 1;
    }

    int NDFT = atoi(argv[1]);
    char *input_file = argv[2];
    char *output_file = argv[3];
    char *mode = argv[4];

    // Allocate memory for input and output buffer
    Complex *inputBuffer = (Complex*) malloc(sizeof(Complex) * NDFT);
    Complex *outputBuffer = (Complex*) malloc(sizeof(Complex) * NDFT);

    // Error handling for input and output buffer allocation
    if (!inputBuffer || !outputBuffer) {
        printf("Buffer allocation failed\n");
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    // Load input data from CSV
    if (!load_data_from_csv(input_file, inputBuffer, NDFT)) {
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    // Compute DFT, IDFT, or CFAR based on mode
    if (strcmp(mode, "dft") == 0) {
        compute_dft(inputBuffer, outputBuffer, NDFT);
        if (!save_data_to_csv(output_file, outputBuffer, NDFT)) {
            free(inputBuffer);
            free(outputBuffer);
            return 1;
        }
    } 
    else if (strcmp(mode, "idft") == 0) {
        compute_idft(inputBuffer, outputBuffer, NDFT);
        if (!save_data_to_csv(output_file, outputBuffer, NDFT)) {
            free(inputBuffer);
            free(outputBuffer);
            return 1;
        }
    } 
    else if (strcmp(mode, "cfar") == 0) {
        // CFAR Mode: Compute DFT first, then apply CFAR
        
        // Check for additional CFAR parameters
        if (argc != 8) {
            printf("Usage for CFAR: %s <number_of_samples> <input_file> <output_file> cfar <threshold_factor> <N_ref> <N_guard>\n", argv[0]);
            free(inputBuffer);
            free(outputBuffer);
            return 1;
        }
        
        // CFAR parameters
        float threshold_factor = atof(argv[5]);
        int N_ref = atoi(argv[6]);
        int N_guard = atoi(argv[7]);

        // Compute DFT
        compute_dft(inputBuffer, outputBuffer, NDFT);
        
        // Allocate memory for squared magnitude and CFAR threshold
        float *squared_magnitude = (float*) malloc(sizeof(float) * NDFT);
        float *cfar_threshold = (float*) malloc(sizeof(float) * NDFT);

        if (!squared_magnitude || !cfar_threshold) {
            printf("Memory allocation failed for CFAR.\n");
            free(inputBuffer);
            free(outputBuffer);
            free(squared_magnitude);
            free(cfar_threshold);
            return 1;
        }

        // Compute squared magnitude of DFT output
        compute_squared_magnitude(outputBuffer, squared_magnitude, NDFT);

        // Apply CFAR
        apply_cfar(squared_magnitude, cfar_threshold, NDFT, N_ref, N_guard, threshold_factor);

        // Save squared magnitude and CFAR threshold to CSV
        if (!save_cfar_to_csv(output_file, squared_magnitude, cfar_threshold, NDFT)) {
            free(inputBuffer);
            free(outputBuffer);
            free(squared_magnitude);
            free(cfar_threshold);
            return 1;
        }

        // Free CFAR-related memory
        free(squared_magnitude);
        free(cfar_threshold);
    } else if (strcmp(mode,"kmeans")==0) {
        // Ensure both the number of clusters and max iterations are provided
        if (argc < 7) {
            printf("Usage for K-means: %s <number_of_samples> <input_file> <output_file> kmeans <num_clusters> <max_iter>\n", argv[0]);
            free(inputBuffer);
            free(outputBuffer);
            return 1;
        }

        int numClusters = atoi(argv[5]);           // Number of clusters
        int max_iter = atoi(argv[6]);    // Max iterations

        compute_dft(inputBuffer, outputBuffer, NDFT);

        // Allocate memory for squared magnitude, labels, and centroids
        float *magnitude = (float*) malloc(sizeof(float) * NDFT);
        int *labels = (int*) malloc(sizeof(int) * NDFT);
        float *centroids = (float*) malloc(sizeof(float) * numClusters);

        if (!magnitude || !labels || !centroids) {
            printf("Memory allocation failed for K-means.\n");
            free(inputBuffer);
            free(outputBuffer);
            free(magnitude);
            free(labels);
            free(centroids);
            return 1;
        }

        compute_squared_magnitude(outputBuffer, magnitude, NDFT);
        //compute_magnitude_dB(outputBuffer, magnitude, NDFT);
        compute_clusters(magnitude, NDFT, numClusters, max_iter, labels, centroids);

        // Save the K-means results to CSV
        save_clusters_to_csv(output_file, magnitude, labels, NDFT);

        // Free memory for K-means
        free(magnitude);
        free(labels);
        free(centroids);

    }
    else {
        printf("Invalid mode. Use 'dft', 'idft', or 'cfar'.\n");
        free(inputBuffer);
        free(outputBuffer);
        return 1;
    }

    // Free input and output buffers
    free(inputBuffer);
    free(outputBuffer);

    printf("Operation '%s' completed. Results saved to %s\n", mode, output_file);
    return 0;
}