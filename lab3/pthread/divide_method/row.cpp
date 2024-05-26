#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <pthread.h>

using namespace std;

#define MAX_N 10000
#define BLOCK_SIZE 128
#define NUM_THREADS 4

float m[MAX_N][MAX_N];
int N;

struct ThreadData {
    int k;
    int start_col;
    int end_col;
};

void m_reset() {
    for(int i = 0; i < N; i++) {
        m[i][i] = 1.0;
        for(int j = i + 1; j < N; j++)
            m[i][j] = (float)(rand() % 10000);
    }
    for(int k = 0; k < N; k++)
        for(int i = k + 1; i < N; i++)
            for(int j = 0; j < N; j++)
                m[i][j] += m[k][j];
}

void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int k = data->k;
    int start_col = data->start_col;
    int end_col = data->end_col;
    for (int j = start_col; j < end_col; j++) {
        for (int i = k + 1; i < N; i++) {
            float factor = m[i][k] / m[k][k];
            m[i][j] -= factor * m[k][j];
        }
    }
    pthread_exit(NULL);
}

void parallel_gaussian_elimination() {
    auto start = std::chrono::high_resolution_clock::now();
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    for (int k = 0; k < N - 1; k++) {
        int cols_per_thread = (N - k) / NUM_THREADS;
        for (int t = 0; t < NUM_THREADS; t++) {
            int start_col = k + 1 + t * cols_per_thread;
            int end_col = k + 1 + (t + 1) * cols_per_thread;
            if (t == NUM_THREADS - 1) {
                end_col = N;
            }
            thread_data[t].k = k;
            thread_data[t].start_col = start_col;
            thread_data[t].end_col = end_col;
            pthread_create(&threads[t], NULL, thread_func, (void*)&thread_data[t]);
        }
        for (int t = 0; t < NUM_THREADS; t++) {
            pthread_join(threads[t], NULL);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << 1000 * elapsed.count() << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }
    N = atoi(argv[1]);
    srand(time(0));
    m_reset();
    parallel_gaussian_elimination();
    return 0;
}
