#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <xmmintrin.h>

using namespace std;

#define MAX_N 10000 // 最大矩阵大小
#define NUM_THREADS 4 // 线程数

float m[MAX_N][MAX_N];
int N;

// 定义线程参数结构
struct ThreadData {
    int k;
    int thread_id;
    int num_threads;
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
    int k = data->k;

    for (int i = k + 1 + data->thread_id; i < N; i += data->num_threads) {
        float factor = m[i][k] / m[k][k];
        
        // 利用 SIMD 进行向量化计算
        __m128 factor_vec = _mm_set1_ps(factor); // 将 factor 复制为 4 个相同的元素
        
        for (int j = k; j < N; j += 4) {
            // 使用 SIMD 指令一次性处理 4 个元素
            __m128 m_kj_vec = _mm_loadu_ps(&m[k][j]); // 加载 m[k][j], m[k][j+1], m[k][j+2], m[k][j+3]
            __m128 m_ij_vec = _mm_loadu_ps(&m[i][j]); // 加载 m[i][j], m[i][j+1], m[i][j+2], m[i][j+3]
            
            // 计算更新后的矩阵元素
            __m128 updated_m_ij_vec = _mm_sub_ps(m_ij_vec, _mm_mul_ps(factor_vec, m_kj_vec));
            
            // 存储更新后的矩阵元素
            _mm_storeu_ps(&m[i][j], updated_m_ij_vec);
        }
    }
    
    pthread_exit(NULL);
}

// 并行化高斯消元
void parallel_gaussian_elimination() {
    auto start = std::chrono::high_resolution_clock::now();
    
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    for (int k = 0; k < N - 1; k++) {
        // 创建线程
        for (int t = 0; t < NUM_THREADS; t++) {
            thread_data[t].k = k;
            thread_data[t].thread_id = t;
            thread_data[t].num_threads = NUM_THREADS;
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

    parallel_gaussian_elimination(); // 执行并行化高斯消元

    return 0;
}
