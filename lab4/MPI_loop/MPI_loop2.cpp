#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <mpi.h>

using namespace std;

#define MAX_N 10000 // 最大矩阵大小

float m[MAX_N][MAX_N];
int N;

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

// 获取行列块的大小
void get_block_size(int size, int* block_rows, int* block_cols) {
    int sqrt_size = (int)sqrt(size);
    while (size % sqrt_size != 0) {
        sqrt_size--;
    }
    *block_rows = sqrt_size;
    *block_cols = size / sqrt_size;
}

// 获取进程负责的行块和列块范围
void get_block_range(int rank, int block_rows, int block_cols, int* r1, int* r2, int* c1, int* c2) {
    int row_block_id = rank / block_cols;
    int col_block_id = rank % block_cols;

    int rows_per_block = (N + block_rows - 1) / block_rows;
    int cols_per_block = (N + block_cols - 1) / block_cols;

    *r1 = row_block_id * rows_per_block;
    *r2 = min(*r1 + rows_per_block, N) - 1;
    *c1 = col_block_id * cols_per_block;
    *c2 = min(*c1 + cols_per_block, N) - 1;
}

// 高斯消元
void gaussian_elimination(int rank, int size) {
    int block_rows, block_cols;
    get_block_size(size, &block_rows, &block_cols);

    int r1, r2, c1, c2;
    get_block_range(rank, block_rows, block_cols, &r1, &r2, &c1, &c2);

    for (int k = 0; k < N; k++) {
        int owner_row = k / ((N + block_rows - 1) / block_rows);
        int owner_col = k / ((N + block_cols - 1) / block_cols);
        int owner_rank = owner_row * block_cols + owner_col;

        // 持有对角线元素的进程进行除法，并广播结果
        if (rank == owner_rank) {
            for (int j = k + 1; j < N; j++) {
                m[k][j] /= m[k][k];
            }
            m[k][k] = 1.0;
            for (int j = 0; j < size; j++) {
                if (j != rank) {
                    MPI_Send(&m[k][0], N, MPI_FLOAT, j, 0, MPI_COMM_WORLD);
                }
            }
        } else {
            MPI_Recv(&m[k][0], N, MPI_FLOAT, owner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // 各进程对自己负责的行块和列块进行消元操作
        for (int i = r1; i <= r2; i++) {
            if (i > k) {
                for (int j = k + 1; j < N; j++) {
                    m[i][j] -= m[k][j] * m[i][k];
                }
                m[i][k] = 0.0;
            }
        }
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
