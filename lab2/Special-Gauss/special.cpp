#include <iostream>
#include <fstream>
#include <vector>
#include <bitset> // 用于位运算
#include <sstream>

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

    // 遍历每个被消元行
    for (int i = 0; i < row_tmp.size(); ++i) {
        // 遍历行中的每个元素
        for (int j = 0; j < row_tmp[i].size(); ++j) {
            // 当前元素不为零，需要处理
            if (row_tmp[i][j] != 0) {
                // 检查是否有对应消元子
                if ((ele_tmp[j / 32][j % 32 / 32] & (1 << (j % 32 % 32))) != 0) {
                    // 有对应消元子，对当前行进行异或
                    for (int p = 0; p < ele_tmp[j].size(); ++p) {
                        row_tmp[i][p] ^= ele_tmp[j][p];
                    }
                } 
                else {
                    // 没有对应消元子，将当前行升格为消元子
                    promoteToEliminationSub(ele_tmp, row_tmp[i]);
                    break;
                }
            }
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
