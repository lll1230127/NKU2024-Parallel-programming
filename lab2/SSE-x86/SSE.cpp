#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <xmmintrin.h>

using namespace std;

#define MAX_N 10000 // Maximum matrix size

float m[MAX_N][MAX_N];
int N;

// Reset matrix
void m_reset() {
    for(int i = 0; i < N; i++) {
        m[i][i] = 1.0;
        for(int j = i + 1; j < N; j++)
            m[i][j] = (float)(rand() % 10000); // Generate random numbers
    }

    // Accumulate lower triangle elements to upper triangle
    for(int k = 0; k < N; k++)
        for(int i = k + 1; i < N; i++)
            for(int j = 0; j < N; j++)
                m[i][j] += m[k][j];
}

// Print matrix
void printMatrix() {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
}

// Gaussian elimination using SSE
void gaussian_elimination_sse() {
    __m128 vt, va, vaik, vakj, vaij, vx;
    auto start = std::chrono::high_resolution_clock::now();

    for(int k = 0; k < N; k++) {
        vt = _mm_set1_ps(m[k][k]);

        // Process the k-th row
        for(int j = k + 1; j < N; j += 8) {
            va = _mm_loadu_ps(&m[k][j]);
            va = _mm_div_ps(va, vt);
            _mm_storeu_ps(&m[k][j], va);
        }
        m[k][k] = 1.0;

        // Process rows from k+1 to N-1
        for(int i = k + 1; i < N; i++) {
            vaik = _mm_set1_ps(m[i][k]);

            // Process the k-th row
            for(int j = k + 1; j < N; j += 8) {
                vakj = _mm_loadu_ps(&m[k][j]);
                vaij = _mm_loadu_ps(&m[i][j]);
                vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_storeu_ps(&m[i][j], vaij);
            }
            m[i][k] = 0.0;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << 1000 * elapsed.count()<< endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    N = atoi(argv[1]); // Get matrix size argument

    srand(time(0)); // Set random seed
    m_reset(); // Generate test data

    // std::cout << "Original Matrix:" << std::endl;
    // printMatrix();

    gaussian_elimination_sse(); // Perform Gaussian elimination using SSE

    // std::cout << "\nUpper Triangular Matrix:" << std::endl;
    // printMatrix();

    return 0;
}
