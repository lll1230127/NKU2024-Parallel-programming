#include <iostream>
#include <fstream>
#include <vector>
#include <bitset> // 用于位运算
#include <sstream>
#include <omp.h> // OpenMP header
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

struct ThreadData {
    vector<vector<unsigned int>>* batchEliminationRows;
    vector<vector<unsigned int>>* eliminationSubs;
    int start;
    int end;
    int col; // 当前处理的列索引
};

void promoteToEliminationSub(vector<vector<unsigned int>>& ele_tmp, vector<unsigned int>& row) {

    //ele_tmp.push_back(row);
}

void specialGaussianElimination(vector<vector<unsigned int>>& batchEliminationRows, vector<vector<unsigned int>>& eliminationSubs) {
    // 打开输出文件
    ofstream outputFile("消元结果.txt");

    // 创建临时副本以防修改原始数据
    vector<vector<unsigned int>> ele_tmp = eliminationSubs;
    vector<vector<unsigned int>> row_tmp = batchEliminationRows;

    for (int j = 0; j < ele_tmp.size(); ++j) {
        if (ele_tmp[j][j] == 1) {
            // 并行处理当前的消元子行
            #pragma omp parallel for
            for (int i = 0; i < row_tmp.size(); ++i) {
                if (row_tmp[i][j] != 0) {
                    for (int p = 0; p < ele_tmp[j].size(); ++p) {
                        row_tmp[i][p] ^= ele_tmp[j][p];
                    }
                }
            }
        } else {
            // 遍历所有被消元行，寻找第一个 j 列有值的行并将其升格为消元子
            for (int i = 0; i < row_tmp.size(); ++i) {
                if (row_tmp[i][j] != 0) {
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
    vector<vector<unsigned int>> eliminationSubs = readMatrixFromFile("消元子.txt");
    vector<vector<unsigned int>> batchEliminationRows = readMatrixFromFile("被消元行.txt");

    specialGaussianElimination(batchEliminationRows, eliminationSubs);

    return 0;
}
