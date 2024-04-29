#include <arm_neon.h>
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

// 打印矩阵
void printMatrix() {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
}

// 高斯消元
void gaussian_elimination() {
    float32x4_t vt, va, vaik, vakj, vaij, vx;
    auto start = std::chrono::high_resolution_clock::now();

    for(int k = 0; k < N; k++) {
        vt = vdupq_n_f32(m[k][k]);

        // 处理第 k 行
        for(int j = k + 1; j < N; j += 4) {
            va = vld1q_f32(&m[k][j]);
            va = vdivq_f32(va, vt);
            vst1q_f32(&m[k][j], va);
        }
        m[k][k] = 1.0;

        // 处理第 k+1 到第 N-1 行
        for(int i = k + 1; i < N; i++) {
            vaik = vdupq_n_f32(m[i][k]);

            // 处理第 k 行
            for(int j = k + 1; j < N; j += 4) {
                vakj = vld1q_f32(&m[k][j]);
                vaij = vld1q_f32(&m[i][j]);
                vx = vmulq_f32(vakj, vaik);
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&m[i][j], vaij);
            }
            m[i][k] = 0.0;
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << 1000 * elapsed.count()<<endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    N = atoi(argv[1]); // 获取矩阵大小参数

    srand(time(0)); // 设置随机种子
    m_reset(); // 生成测试数据

    // std::cout << "Original Matrix:" << std::endl;
    // printMatrix();

    gaussian_elimination(); // 执行高斯消元

    // std::cout << "\nUpper Triangular Matrix:" << std::endl;
    // printMatrix();

    return 0;
}
