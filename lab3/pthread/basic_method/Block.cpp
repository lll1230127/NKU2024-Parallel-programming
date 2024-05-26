#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <pthread.h>

using namespace std;

#define MAX_N 10000 // 最大矩阵大小
#define BLOCK_SIZE 32 // 块大小
#define NUM_THREADS 4 // 线程数

float m[MAX_N][MAX_N];
int N;

// 定义线程参数结构
struct ThreadData {
    int row_start;
    int row_end;
};

// 重置矩阵
void m_reset() {
    for(int i = 0; i < N; i++) {
        m[i][i] = 1.0;
        for(int j = i + 1; j < N; j++)
            m[i][j] = (float)(rand() % 10000); // 生成随机数
    }

    // 累加下三角元素到上三角
    for(int k = 0; k < N; k++)
        for(int i = k + 1; i < N; i++)
            for(int j = 0; j < N; j++)
                m[i][j] += m[k][j];
}

// 打印矩阵
void printMatrix() {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
}

// 线程函数
void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int row_start = data->row_start;
    int row_end = data->row_end;
    
    for (int k = 0; k < N; k++) {
        for (int i = row_start; i < row_end; i++) {
            if (i > k) {
                float factor = m[i][k] / m[k][k];
                for (int j = k; j < N; j++) {
                    m[i][j] -= factor * m[k][j];
                }
            }
        }
    }
    
    pthread_exit(NULL);
}

// 分块并行化高斯消元
void block_parallel_gaussian_elimination() {
    auto start = std::chrono::high_resolution_clock::now();
    
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    for (int k = 0; k < N; k += BLOCK_SIZE) {
        // 创建线程
        for (int t = 0; t < NUM_THREADS; t++) {
            thread_data[t].row_start = k + t * (BLOCK_SIZE / NUM_THREADS);
            thread_data[t].row_end = min(k + (t + 1) * (BLOCK_SIZE / NUM_THREADS), N);
            pthread_create(&threads[t], NULL, thread_func, (void*)&thread_data[t]);
        }
        
        // 等待所有线程完成
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

    N = atoi(argv[1]); // 获取矩阵大小参数

    srand(time(0)); // 设置随机种子
    m_reset(); // 生成测试数据

    block_parallel_gaussian_elimination(); // 执行分块并行化高斯消元

    return 0;
}
