#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cstdio>

// typedef std::function<void* (void*)> fun_t;
typedef void *(*fun_t)(void *);

class ThreadData
{
public:
    void *args_;
    std::string name_;
};

class Thread
{
public:
    Thread(int num, fun_t callback, void *args) : func_(callback)
    {
        char nameBuffer[64];
        snprintf(nameBuffer, sizeof nameBuffer, "Thread-%d", num);
        name_ = nameBuffer;

        tdata_.args_ = args;
        tdata_.name_ = name_;
    }
    void start()
    {
        pthread_create(&tid_, nullptr, func_, (void*)&tdata_);
    }
    void join()
    {
        pthread_join(tid_, nullptr);
    }
    std::string name()
    {
        return name_;
    }
    ~Thread()
    {
    }

private:
    std::string name_;
    fun_t func_;
    ThreadData tdata_;
    pthread_t tid_;
};