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
    int k;
    int thread_id;
    sem_t* sem_workerstart;
    sem_t* sem_main;
    sem_t* sem_workerend;
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

// 打印矩阵
void printMatrix() {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++)
            std::cout << m[i][j] << " ";
        std::cout << std::endl;
    }
}

// 线程函数
void* thread_func(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int k = data->k;
    int thread_id = data->thread_id;
    sem_t* sem_workerstart = data->sem_workerstart;
    sem_t* sem_main = data->sem_main;
    sem_t* sem_workerend = data->sem_workerend;
    
    while(true) {
        sem_wait(sem_workerstart);
        
        // 消去操作
        for (int j = k + 1; j < N; j++) {
            m[k][j] /= m[k][k];
        }
        m[k][k] = 1.0;
        
        sem_post(sem_main);
        
        sem_wait(sem_workerend);
    }
    
    pthread_exit(NULL);
}

// 并行化高斯消元
void parallel_gaussian_elimination() {
    auto start = std::chrono::high_resolution_clock::now();
    
    sem_t sem_workerstart[NUM_THREADS];
    sem_t sem_main;
    sem_t sem_workerend[NUM_THREADS];
    
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    
    // 初始化信号量
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_init(&sem_workerstart[i], 0, 0);
        sem_init(&sem_workerend[i], 0, 0);
    }
    sem_init(&sem_main, 0, 0);
    
    // 创建线程
    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
        thread_data[t_id].k = 0;
        thread_data[t_id].thread_id = t_id;
        thread_data[t_id].sem_workerstart = &sem_workerstart[t_id];
        thread_data[t_id].sem_main = &sem_main;
        thread_data[t_id].sem_workerend = &sem_workerend[t_id];
        pthread_create(&threads[t_id], NULL, thread_func, (void*)&thread_data[t_id]);
    }
    
    for (int k = 0; k < N - 1; ++k) {
        // 主线程做除法操作
        for (int j = k + 1; j < N; ++j) {
            m[k][j] /= m[k][k];
        }
        m[k][k] = 1.0;
        
        // 开始唤醒工作线程
        for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
            sem_post(&sem_workerstart[t_id]);
        }
        
        // 主线程睡眠（等待所有的工作线程完成此轮消去任务）
        for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
            sem_wait(&sem_main);
        }
        
        // 主线程再次唤醒工作线程进入下一轮次的消去任务
        for (int t_id = 0; t_id < NUM_THREADS; ++t_id) {
            sem_post(&sem_workerend[t_id]);
        }
    }
    
    // 销毁所有信号量
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_destroy(&sem_workerstart[i]);
        sem_destroy(&sem_workerend[i]);
    }
    sem_destroy(&sem_main);
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << 1000 * elapsed.count() << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " N\n";
        return 1;
    }

    N = atoi(argv[1]); // 获取矩阵大小参数

    srand(time(0)); // 设置随机种子
    m_reset(); // 生成测试数据

    parallel_gaussian_elimination(); // 执行并行化高斯消元

    return 0;
}
