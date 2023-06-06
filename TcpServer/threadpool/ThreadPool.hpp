#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unistd.h>
#include "thread.hpp"
#include "LockGuard.hpp"
#include "log.hpp"

const int g_thread_num = 10;
// 本质是: 生产消费模型
template <class T>
class ThreadPool
{
public:
    pthread_mutex_t *getMutex()
    {
        return &lock;
    }
    bool isEmpty()
    {
        return task_queue_.empty();
    }
    void waitCond()
    {
        pthread_cond_wait(&cond, &lock);
    }
    T getTask()
    {
        T t = task_queue_.front();
        task_queue_.pop();
        return t;
    }

private:
    ThreadPool(int thread_num = g_thread_num) : num_(thread_num)
    {
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&cond, nullptr);
        for (int i = 1; i <= num_; i++)
        {
            threads_.push_back(new Thread(i, routine, this));
        }
    }
    ThreadPool(const ThreadPool<T> &other) = delete;
    const ThreadPool<T> &operator=(const ThreadPool<T> &other) = delete;

public:
    // 考虑一下多线程使用单例的过程
    static ThreadPool<T> *getThreadPool(int num = g_thread_num)
    {
        // 可以有效减少未来必定要进行加锁检测的问题
        // 拦截大量的在已经创建好单例的时候，剩余线程请求单例的而直接访问锁的行为
        if (nullptr == thread_ptr) 
        {
            lockGuard lockguard(&mutex);
            // 但是，未来任何一个线程想获取单例，都必须调用getThreadPool接口
            // 但是，一定会存在大量的申请和释放锁的行为，这个是无用且浪费资源的
            // pthread_mutex_lock(&mutex);
            if (nullptr == thread_ptr)
            {
                thread_ptr = new ThreadPool<T>(num);
            }
            // pthread_mutex_unlock(&mutex);
        }
        return thread_ptr;
    }
    // 1. run()
    void run()
    {
        for (auto &iter : threads_)
        {
            iter->start();
            // std::cout << iter->name() << " 启动成功" << std::endl;
            logMessage(NORMAL, "%s %s", iter->name().c_str(), "启动成功");
        }
    }
    // 线程池本质也是一个生产消费模型
    // void *routine(void *args)
    // 消费过程
    static void *routine(void *args)
    {
        ThreadData *td = (ThreadData *)args;
        ThreadPool<T> *tp = (ThreadPool<T> *)td->args_;
        while (true)
        {
            T task;
            {
                lockGuard lockguard(tp->getMutex());
                while (tp->isEmpty())
                    tp->waitCond();
                // 读取任务
                task = tp->getTask(); // 任务队列是共享的-> 将任务从共享，拿到自己的私有空间
            }
            task(td->name_);
            // lock
            // while(task_queue_.empty()) wait();
            // 获取任务
            // unlock

            // 处理任务
        }
    }
    // 2. pushTask()
    void pushTask(const T &task)
    {
        lockGuard lockguard(&lock);
        task_queue_.push(task);
        pthread_cond_signal(&cond);
    }
    // test func
    // void joins()
    // {
    //     for (auto &iter : threads_)
    //     {
    //         iter->join();
    //     }
    // }
    ~ThreadPool()
    {
        for (auto &iter : threads_)
        {
            iter->join();
            delete iter;
        }
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }

private:
    std::vector<Thread *> threads_;
    int num_;
    std::queue<T> task_queue_;

    static ThreadPool<T> *thread_ptr;
    static pthread_mutex_t mutex;

    // 方案2:
    //  queue1,queue2
    //  std::queue<T> *p_queue, *c_queue
    //  p_queue->queue1
    //  c_queue->queue2
    //  p_queue -> 生产一批任务之后，swap(p_queue,c_queue),唤醒所有线程/一个线程
    //  当消费者处理完毕的时候，你也可以进行swap(p_queue,c_queue)
    //  因为我们生产和消费用的是不同的队列，未来我们要进行资源的处理的时候，仅仅是指针

    pthread_mutex_t lock;
    pthread_cond_t cond;
};
template <typename T>
ThreadPool<T> *ThreadPool<T>::thread_ptr = nullptr;

template <typename T>
pthread_mutex_t ThreadPool<T>::mutex = PTHREAD_MUTEX_INITIALIZER;