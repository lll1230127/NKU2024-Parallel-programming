#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void generateRandomMatrix(float** matrix, float* vector, int n) {
    srand(time(NULL));
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            matrix[row][col] = (float)rand() / RAND_MAX;
        }
        vector[row] = (float)rand() / RAND_MAX;
    }
}

double calculateDotProduct(float** matrix, float* vector, int n) {
    float* dotProduct = (float*)malloc(n * sizeof(float));
    
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start); // Start the timer
    
    for (int col = 0; col < n; col++) {
        dotProduct[col]  = 0;
        for (int row = 0; row < n; row++) {
            dotProduct[col] += matrix[row][col] * vector[row];
        }
    }
    
    QueryPerformanceCounter(&end); // Stop the timer
    double time_taken = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart; // Calculate the time taken
    
    return time_taken;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    
    float** matrix = (float**)malloc(n * sizeof(float*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (float*)malloc(n * sizeof(float));
    }
    
    float* vector = (float*)malloc(n * sizeof(float));
    
    generateRandomMatrix(matrix, vector, n);

    double sum = 0;
    int iters = 6;
    for (int i = 0; i < iters; i++) {
        sum += calculateDotProduct(matrix, vector, n);
    }
    if(n==200){
        printf("| Iteration | Time Taken (seconds) |\n");
        printf("|-----------|----------------------|\n");
    }
    printf("| %d         | %f             |\n", n, sum / iters);
    
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
    free(vector);
    
    return 0;
}