#include <iostream>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define MAX_N 10000 // 最大矩阵大小
#define NUM_THREADS 4 // 线程数

float m[MAX_N][MAX_N];
int N;

// 定义线程参数结构
struct ThreadData {
    int thread_id;
    sem_t* sem_Divsion;
    sem_t* sem_Elimination;
    sem_t* sem_leader;
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

// 线程函数
void* threadFunc(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int t_id = data->thread_id;
    sem_t* sem_Divsion = data->sem_Divsion;
    sem_t* sem_Elimination = data->sem_Elimination;
    sem_t* sem_leader = data->sem_leader;
    
    for (int k = 0; k < N; ++k) {
        // t_id为0的线程执行除法操作，其余线程等待
        if (t_id == 0) {
            for (int j = k + 1; j < N; ++j) {
                m[k][j] /= m[k][k];
            }
            m[k][k] = 1.0;
        } else {
            sem_wait(&sem_Divsion[t_id - 1]);
        }

        // 唤醒其他线程进行消去操作
        if (t_id == 0) {
            for (int i = 0; i < NUM_THREADS - 1; ++i) {
                sem_post(&sem_Divsion[i]);
            }
        }
        
        // 分配任务并执行消去操作
        for (int i = k + 1 + t_id; i < N; i += NUM_THREADS) {
            for (int j = k + 1; j < N; ++j) {
                m[i][j] -= m[i][k] * m[k][j];
            }
            m[i][k] = 0.0;
        }

        // 主线程等待其他线程完成消去操作
        if (t_id == 0) {
            for (int i = 0; i < NUM_THREADS - 1; ++i) {
                sem_wait(sem_leader);
            }
            
            // 通知其他线程进入下一轮
            for (int i = 0; i < NUM_THREADS - 1; ++i) {
                sem_post(&sem_Elimination[i]);
            }
        } else {
            // 通知主线程已完成消去操作
            sem_post(sem_leader);
            
            // 等待主线程通知，进入下一轮
            sem_wait(&sem_Elimination[t_id - 1]);
        }
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    N = atoi(argv[1]); // 获取矩阵大小参数

    srand(time(0)); // 设置随机种子
    m_reset(); // 生成测试数据
        auto start = std::chrono::high_resolution_clock::now();
    // 初始化信号量
    sem_t sem_leader;
    sem_t sem_Divsion[NUM_THREADS - 1];
    sem_t sem_Elimination[NUM_THREADS - 1];

    sem_init(&sem_leader, 0, 0);
    for (int i = 0; i < NUM_THREADS - 1; ++i) {
        sem_init(&sem_Divsion[i], 0, 0);
        sem_init(&sem_Elimination[i], 0, 0);
    }

    // 创建线程
    pthread_t handles[NUM_THREADS];
    ThreadData param[NUM_THREADS];
    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        param[t_id].thread_id = t_id;
        param[t_id].sem_Divsion = sem_Divsion;
        param[t_id].sem_Elimination = sem_Elimination;
        param[t_id].sem_leader = &sem_leader;
        pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
    }

    // 等待线程完成
    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        pthread_join(handles[t_id], NULL);
    }

    // 销毁信号量
    sem_destroy(&sem_leader);
    for (int i = 0; i < NUM_THREADS - 1; ++i) {
        sem_destroy(&sem_Divsion[i]);
        sem_destroy(&sem_Elimination[i]);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << 1000 * elapsed.count() << endl;
    return 0;
}
