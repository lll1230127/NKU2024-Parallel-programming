#include <iostream>
#include <fstream>
#include <vector>
#include <bitset> // ����λ����
#include <sstream>

using namespace std;

// ���ļ��ж�ȡ��������
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

// ������Ԫ������Ϊ��Ԫ��
void promoteToEliminationSub(vector<vector<unsigned int>>& eliminationSubs, vector<unsigned int>& row) {
    eliminationSubs.push_back(row);
}

// �����˹��Ԫ�㷨
void specialGaussianElimination(vector<vector<unsigned int>>& batchEliminationRows, vector<vector<unsigned int>>& eliminationSubs) {
    // ������ļ�
    ofstream outputFile("��Ԫ���.txt");

    // ������ʱ�����Է��޸�ԭʼ����
    vector<vector<unsigned int>> ele_tmp = eliminationSubs;
    vector<vector<unsigned int>> row_tmp = batchEliminationRows;

    // ����ÿ������Ԫ��
    for (int i = 0; i < row_tmp.size(); ++i) {
        // �������е�ÿ��Ԫ��
        for (int j = 0; j < row_tmp[i].size(); ++j) {
            // ��ǰԪ�ز�Ϊ�㣬��Ҫ����
            if (row_tmp[i][j] != 0) {
                // ����Ƿ��ж�Ӧ��Ԫ��
                if ((ele_tmp[j / 32][j % 32 / 32] & (1 << (j % 32 % 32))) != 0) {
                    // �ж�Ӧ��Ԫ�ӣ��Ե�ǰ�н������
                    for (int p = 0; p < ele_tmp[j].size(); ++p) {
                        row_tmp[i][p] ^= ele_tmp[j][p];
                    }
                } 
                else {
                    // û�ж�Ӧ��Ԫ�ӣ�����ǰ������Ϊ��Ԫ��
                    promoteToEliminationSub(ele_tmp, row_tmp[i]);
                    break;
                }
            }
        }
    }


    // ���������ļ�
    for (const auto& row : row_tmp) {
        for (size_t element : row) {
            outputFile << element << " ";
        }
        outputFile << endl;
    }

    // �ر�����ļ�
    outputFile.close();
}

int main() {
    // ���ļ��ж�ȡ��Ԫ�Ӻͱ���Ԫ�еľ�������
    vector<vector<unsigned int>> eliminationSubs = readMatrixFromFile("��Ԫ��.txt");
    vector<vector<unsigned int>> batchEliminationRows = readMatrixFromFile("����Ԫ��.txt");

    // ִ�������˹��ȥ�㷨
    specialGaussianElimination(batchEliminationRows, eliminationSubs);

    return 0;
}
