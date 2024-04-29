#include <immintrin.h>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>

using namespace std;

#define MAX_N 10000 // 最大矩阵大小

float m[MAX_N][MAX_N];
int N;

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

// 高斯消元
void gaussian_elimination() {
    auto start = std::chrono::high_resolution_clock::now();

    for(int k = 0; k < N; k++) {
        float vt = m[k][k];

        // 处理第 k 行
        for(int j = k + 1; j < N; j += 16) {
            __m512 vt_avx = _mm512_set1_ps(vt);
            __m512 va = _mm512_load_ps(&m[k][j]);
            va = _mm512_div_ps(va, vt_avx);
            _mm512_store_ps(&m[k][j], va);
        }
        m[k][k] = 1.0;

        // 处理第 k+1 到第 N-1 行
        for(int i = k + 1; i < N; i++) {
            float vaik = m[i][k];

            // 处理第 k 行
            for(int j = k + 1; j < N; j += 16) {
                __m512 vaik_avx = _mm512_set1_ps(vaik);
                __m512 vakj = _mm512_load_ps(&m[k][j]);
                __m512 vaij = _mm512_load_ps(&m[i][j]);
                __m512 vx = _mm512_mul_ps(vakj, vaik_avx);
                vaij = _mm512_sub_ps(vaij, vx);
                _mm512_store_ps(&m[i][j], vaij);
            }
            m[i][k] = 0.0;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Execution time: " << 1000 * elapsed.count() << " ms\n";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    N = atoi(argv[1]); // 获取矩阵大小参数

    srand(time(0)); // 设置随机种子
    m_reset(); // 生成测试数据

    gaussian_elimination(); // 执行高斯消元

    return 0;
}
