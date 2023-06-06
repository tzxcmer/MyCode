#pragma once

#include <iostream>
#include <cassert>
#include <queue>
#include <memory>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "Lock.hpp"

using namespace std;

int gThreadNum = 15;

template <class T>
class ThreadPool
{
private:
    ThreadPool(int threadNum = gThreadNum) : threadNum_(threadNum), isStart_(false)
    {
        assert(threadNum_ > 0);
        pthread_mutex_init(&mutex_, nullptr);
        pthread_cond_init(&cond_, nullptr);
    }
    ThreadPool(const ThreadPool<T> &) = delete;
    void operator=(const ThreadPool<T>&) = delete;

public:
    static ThreadPool<T> *getInstance()
    {
        static Mutex mutex;
        if (nullptr == instance) //仅仅是过滤重复的判断
        {
            LockGuard lockguard(&mutex); //进入代码块，加锁。退出代码块，自动解锁
            if (nullptr == instance)
            {
                instance = new ThreadPool<T>();
            }
        }

        return instance;
    }
    //类内成员, 成员函数，都有默认参数this
    static void *threadRoutine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);
        // prctl(PR_SET_NAME, "follower"); // 更改线程名称
        while (1)
        {
            tp->lockQueue();
            while (!tp->haveTask())
            {
                tp->waitForTask();
            }
            //这个任务就被拿到了线程的上下文中
            T t = tp->pop();
            tp->unlockQueue();
            t(); // 让指定的先处理这个任务
        }
    }
    void start()
    {
        assert(!isStart_);
        for (int i = 0; i < threadNum_; i++)
        {
            pthread_t temp;
            pthread_create(&temp, nullptr, threadRoutine, this);
        }
        isStart_ = true;
    }
    void push(const T &in)
    {
        lockQueue();
        taskQueue_.push(in);
        choiceThreadForHandler();
        unlockQueue();
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);
    }
    int threadNum()
    {
        return threadNum_;
    }

private:
    void lockQueue() { pthread_mutex_lock(&mutex_); }
    void unlockQueue() { pthread_mutex_unlock(&mutex_); }
    bool haveTask() { return !taskQueue_.empty(); }
    void waitForTask() { pthread_cond_wait(&cond_, &mutex_); }
    void choiceThreadForHandler() { pthread_cond_signal(&cond_); }
    T pop()
    {
        T temp = taskQueue_.front();
        taskQueue_.pop();
        return temp;
    }

private:
    bool isStart_;
    int threadNum_;
    queue<T> taskQueue_;
    pthread_mutex_t mutex_;
    pthread_cond_t cond_;

    static ThreadPool<T> *instance;
    // const static int a = 100;
};

template <class T>
ThreadPool<T> *ThreadPool<T>::instance = nullptr;