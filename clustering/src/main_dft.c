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

// K-means clustering function
void k_means(float *data, int N, int k, int max_iter, int *labels, float *centroids) {
    int changes;

    // Initialize centroids with the first k data points
    for (int i = 0; i < k; i++) {
        centroids[i] = data[i];
    }

    for (int iter = 0; iter < max_iter; iter++) {
        changes = 0;

        // Assign each point to the nearest centroid
        for (int i = 0; i < N; i++) {
            int best_cluster = 0;
            float min_dist = fabs(data[i] - centroids[0]);
            for (int j = 1; j < k; j++) {
                float dist = fabs(data[i] - centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_cluster = j;
                }
            }
            if (labels[i] != best_cluster) {
                labels[i] = best_cluster;
                changes++;
            }
        }

        // Check for convergence
        if (changes == 0) break;

        // Update centroids
        float sum[k];
        int count[k];
        memset(sum, 0, sizeof(sum));
        memset(count, 0, sizeof(count));

        for (int i = 0; i < N; i++) {
            sum[labels[i]] += data[i];
            count[labels[i]]++;
        }

        for (int j = 0; j < k; j++) {
            if (count[j] > 0) {
                centroids[j] = sum[j] / count[j];
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

        int k = atoi(argv[5]);           // Number of clusters
        int max_iter = atoi(argv[6]);    // Max iterations

        compute_dft(inputBuffer, outputBuffer, NDFT);

        // Allocate memory for squared magnitude, labels, and centroids
        float *magnitude = (float*) malloc(sizeof(float) * NDFT);
        int *labels = (int*) malloc(sizeof(int) * NDFT);
        float *centroids = (float*) malloc(sizeof(float) * k);

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
        k_means(magnitude, NDFT, k, max_iter, labels, centroids);

        // Save the K-means results to CSV
        save_kmeans_to_csv(output_file, magnitude, labels, NDFT);

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