#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define n 5000

void generateRandomMatrix(float** matrix, float* vector) {
    srand(time(NULL));
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            matrix[row][col] = (float)rand() / RAND_MAX;
        }
        vector[row] = (float)rand() / RAND_MAX;
    }
}

double calculateDotProduct(float** matrix, float* vector) {
    float dotProduct;
    
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start); // Start the timer
    
    for (int col = 0; col < n; col++) {
        dotProduct = 0;
        for (int row = 0; row < n; row++) {
            dotProduct += matrix[row][col] * vector[row];
        }
    }
    
    QueryPerformanceCounter(&end); // Stop the timer
    double time_taken = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // Calculate the time taken
    
    printf("Time taken: %f seconds\n", time_taken);
    return time_taken;
}

int main() {
    
    float** matrix = (float**)malloc(n * sizeof(float*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (float*)malloc(n * sizeof(float));
    }
    
    float* vector = (float*)malloc(n * sizeof(float));
    
    generateRandomMatrix(matrix, vector);

    double sum = 0;
    int iters = 10;
    for (int i = 0; i < iters; i++) {
        sum += calculateDotProduct(matrix, vector);
    }
    printf("All Time taken: %f seconds\n", sum / iters);
    
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
    
    return 0;
}