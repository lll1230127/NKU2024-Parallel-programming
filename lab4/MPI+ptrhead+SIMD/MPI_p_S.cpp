#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <pthread.h>
#include <xmmintrin.h> // SSE头文件

using namespace std;

#define MAX_N 10000 // 最大矩阵大小

float m[MAX_N][MAX_N];
int N;
int num_threads = 4; // 可以根据需要修改线程数量

struct ThreadData {
    int start;
    int end;
};

// 重置矩阵
void m_reset() {
    for (int i = 0; i < N; i++) {
        m[i][i] = 1.0;
        for (int j = i + 1; j < N; j++)
            m[i][j] = (float)(rand() % 10000); // 生成随机数
    }

    // 累加下三角元素到上三角
    for (int k = 0; k < N; k++)
        for (int i = k + 1; i < N; i++)
            for (int j = 0; j < N; j++)
                m[i][j] += m[k][j];
}

// 打印矩阵
void printMatrix() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
}

// 线程函数
void *gaussian_elimination_thread(void *data) {
    ThreadData *thread_data = (ThreadData *)data;
    int start = thread_data->start;
    int end = thread_data->end;

    for (int k = 0; k < N; k++) {
        for (int i = start; i <= end; i++) {
            if (i > k) {
                __m128 mk = _mm_set1_ps(m[k][k]); // 将m[k][k]复制为一个向量
                __m128 *mi = (__m128 *)m[i]; // 将m[i]视为一个向量数组
                __m128 *mj = (__m128 *)m[k]; // 将m[k]视为一个向量数组

                // 使用SSE指令并行计算
                for (int j = k + 1; j < N; j += 4) {
                    __m128 mki = _mm_set1_ps(m[i][j]); // 将m[i][j]复制为一个向量
                    __m128 temp = _mm_mul_ps(mki, mk); // 执行向量乘法
                    __m128 mji = _mm_load_ps((float *)&mj[j]); // 从内存加载向量
                    mji = _mm_sub_ps(mji, temp); // 执行向量减法
                    _mm_store_ps((float *)&mj[j], mji); // 将结果写回内存
                }
                m[i][k] = 0.0;
            }
        }
    }

    pthread_exit(NULL);
}

// 高斯消元
void gaussian_elimination(int rank, int size) {
    int rows_per_process = N / size;
    int r1 = rank * rows_per_process;
    int r2 = (rank + 1) * rows_per_process - 1;
    if (rank == size - 1) r2 = N - 1; // 最后一个进程可能多一些行

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    for (int i = 0; i < num_threads; i++) {
        int start = r1 + (i * (r2 - r1 + 1)) / num_threads;
        int end = r1 + ((i + 1) * (r2 - r1 + 1)) / num_threads - 1;
        thread_data[i] = {start, end};
        pthread_create(&threads[i], NULL, gaussian_elimination_thread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " N\n";
        }
        MPI_Finalize();
        return 1;
    }

    N = atoi(argv[1]); // 获取矩阵大小参数

    if (N > MAX_N) {
        if (rank == 0) {
            std::cerr << "N exceeds maximum allowed size of " << MAX_N << "\n";
        }
        MPI_Finalize();
        return 1;
    }

    srand(time(0)); // 设置随机种子

    if (rank == 0) {
        m_reset(); // 生成测试数据
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 广播矩阵数据到所有进程
    MPI_Bcast(&m[0][0], MAX_N * MAX_N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    gaussian_elimination(rank, size); // 执行高斯消元

    // 收集结果到0号进程
    MPI_Gather(&m[rank * (N / size)][0], (N / size) * N, MPI_FLOAT, &m[0][0], (N / size) * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    
    if (rank == 0) {
        std::cout << 1000 * elapsed.count() << std::endl;
    }

    MPI_Finalize();
    return 0;
}
