#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

void generateRandomArray(int* array, int n) {
    srand(time(NULL));
    
    for (int i = 0; i < n; i++) {
        array[i] = rand();
    }
}

double calculateSum(int* array, int n) {
    int sum = 0;
    
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start); // Start the timer
    
    int temp = 0;
    for (int i = 0; i < n; i++) {
        temp += array[i];
        sum += temp;
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
    
    int* array = (int*)malloc(n * sizeof(int));
    
    generateRandomArray(array, n);

    double sum = 0;
    int iters =100;
    for (int i=0;i<iters;i++) sum+=calculateSum(array, n);
    if(n==200){
        printf("| Iteration | Time Taken (seconds) |\n");
        printf("|-----------|----------------------|\n");
    }
    printf("| %d         | %f             |\n", n, sum*1000);
    
    free(array);
    
    return 0;
}
