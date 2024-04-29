#include <iostream>
#include <fstream>
#include <vector>
#include <bitset> // 用于位运算
#include <sstream>
#include <arm_neon.h> // NEON头文件

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

// 将被消元行升格为消元子
void promoteToEliminationSub(vector<vector<unsigned int>>& eliminationSubs, vector<unsigned int>& row) {
    eliminationSubs.push_back(row);
}

// 特殊高斯消元算法
void specialGaussianElimination(vector<vector<unsigned int>>& batchEliminationRows, vector<vector<unsigned int>>& eliminationSubs) {
    // 打开输出文件
    ofstream outputFile("消元结果.txt");

    // 创建临时副本以防修改原始数据
    vector<vector<unsigned int>> ele_tmp = eliminationSubs;
    vector<vector<unsigned int>> row_tmp = batchEliminationRows;

    // 定义NEON向量类型
    typedef uint32x4_t uint32x4_t;

    // 遍历每个被消元行
    for (int i = 0; i < row_tmp.size(); ++i) {
        // 遍历行中的每个元素，使用NEON加速
        for (int j = 0; j < row_tmp[i].size(); j += 4) {
            // 加载当前行和对应消元子到NEON寄存器
            uint32x4_t rowVec = vld1q_u32(&row_tmp[i][j]);
            uint32x4_t eleVec = vld1q_u32(&ele_tmp[j / 32][j]);

            // 计算NEON向量的按位与
            uint32x4_t andResult = vandq_u32(rowVec, eleVec);

            // 将结果存回内存
            vst1q_u32(&row_tmp[i][j], andResult);
        }
    }

    // 输出结果到文件
    for (const auto& row : row_tmp) {
        for (size_t element : row) {
            outputFile << element << " ";
        }
        outputFile << endl;
    }

    // 关闭输出文件
    outputFile.close();
}

int main() {
    // 从文件中读取消元子和被消元行的矩阵数据
    vector<vector<unsigned int>> eliminationSubs = readMatrixFromFile("消元子.txt");
    vector<vector<unsigned int>> batchEliminationRows = readMatrixFromFile("被消元行.txt");

    // 执行特殊高斯消去算法
    specialGaussianElimination(batchEliminationRows, eliminationSubs);

    return 0;
}
