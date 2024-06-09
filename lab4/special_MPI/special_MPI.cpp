#include <iostream>
#include <fstream>
#include <vector>
#include <bitset> // 用于位运算
#include <sstream>
#include <mpi.h> // 引入MPI库

using namespace std;

// 从文件中读取矩阵数据
vector<vector<unsigned int>> readMatrixFromFile(const string& filename) {
    vector<vector<unsigned int>> matrix;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<unsigned int> row;
        unsigned int val;
        while (ss >> val) {
            row.push_back(val);
        }
        matrix.push_back(row);
    }
    file.close();
    return matrix;
}

// 特殊高斯消元算法
void specialGaussianElimination(vector<vector<unsigned int>>& eliminationSubs, vector<vector<unsigned int>>& batchEliminationRows) {
    // 创建临时副本以防修改原始数据
    vector<vector<unsigned int>> ele_tmp = eliminationSubs;
    vector<vector<unsigned int>> row_tmp = batchEliminationRows;

    // MPI初始化
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 计算每个进程负责处理的消元子的数量
    int subs_per_process = eliminationSubs.size() / size;
    int start_sub = rank * subs_per_process;
    int end_sub = (rank == size - 1) ? eliminationSubs.size() : (rank + 1) * subs_per_process;

    // 每个进程处理一部分消元子所对应的被消元行
    for (int j = start_sub; j < end_sub; ++j) {
        for (int i = 0; i < row_tmp.size(); ++i) {
            // 当前元素不为零，需要处理
            if (row_tmp[i][j] != 0) {
                // 检查是否有对应消元子
                if ((ele_tmp[j][j] & (1 << i)) != 0) {
                    // 有对应消元子，对当前行进行异或
                    for (int p = 0; p < ele_tmp[j].size(); ++p) {
                        row_tmp[i][p] ^= ele_tmp[j][p];
                    }
                } 
                else {
                    // 没有对应消元子，将当前行升格为消元子
                    ele_tmp[j][j] |= (1 << i);
                    break;
                }
            }
        }
    }

    // 汇总每个进程的处理结果到Master进程
    vector<vector<vector<unsigned int>>> all_partial_results(size);
    MPI_Gather(&row_tmp[0][0], row_tmp.size() * row_tmp[0].size(), MPI_UNSIGNED, &all_partial_results[0][0][0], row_tmp.size() * row_tmp[0].size(), MPI_UNSIGNED, 0, MPI_COMM_WORLD);

    // Master进程将结果汇总并输出
    if (rank == 0) {
        // 汇总所有部分结果
        vector<vector<unsigned int>> final_result;
        for (const auto& partial_result : all_partial_results) {
            final_result.insert(final_result.end(), partial_result.begin(), partial_result.end());
        }

        // 输出结果到文件
        ofstream outputFile("消元结果.txt");
        for (const auto& row : final_result) {
            for (size_t element : row) {
                outputFile << element << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // 初始化MPI环境

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 获取当前进程的排名

    // 从文件中读取消元子和被消元行的矩阵数据
    vector<vector<unsigned int>> eliminationSubs, batchEliminationRows;
    if (rank == 0) {
        eliminationSubs = readMatrixFromFile("消元子.txt");
        batchEliminationRows = readMatrixFromFile("被消元行.txt");
    }

    // 执行特殊高斯消去算法
    specialGaussianElimination(eliminationSubs, batchEliminationRows);

    MPI_Finalize(); // 结束MPI环境

    return 0;
}

