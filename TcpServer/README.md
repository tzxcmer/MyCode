# 基于TCP协议的简易网络计算器

首先，你得有这些预备基础知识：

Linux文件系统相关知识，套接字编程，系统调用，进程相关知识，线程相关知识，了解客户端和服务端

本文还会详细介绍一种分等级日志的写法，可大大方便我们进行调试和分析

## 一、分级别日志

日志是帮助我们进行程序分析，对于程序不同的错误是分级别的，比如严重需要立即修改的错误或可以忽略的微小错误，因此我们需要对这些不同的内容进行等级划分

```c++
//不同情况的划分
#define DEBUG 0
#define NOTICE 1
#define WARINING 2
#define FATAL 3

const char *log_level[] = {"DEBUG", "NOTICE", "WARINING", "FATAL"};

#define LOGFILE "serverTcp.log" //存储日志的文件
```

### 日志类的定义

```c++
class Log
{
public:
    Log():logFd(-1)
    {}
    void enable()
    {
        umask(0);
        logFd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
        assert(logFd != -1);
        dup2(logFd, 1);
        dup2(logFd, 2);
    }
    ~Log()
    {
        if(logFd != -1) 
        {
            fsync(logFd);
            close(logFd);
        }
    }


private:
    int logFd;
};
```

利用类的构造和析构来控制文件的打开与关闭，此处利用了些RAII的思想，简化了每次打印日志都要打开文件的操作

### 日志显示的函数定义

```c++
// logMessage(DEBUG, "%d", 10);
void logMessage(int level, const char *format, ...)
{
    assert(level >= DEBUG);
    assert(level <= FATAL);

    char *name = getenv("USER");

    char logInfo[1024];
    va_list ap; // ap -> char*
    va_start(ap, format);//获取可变参数列表的第一个参数的地址

    vsnprintf(logInfo, sizeof(logInfo) - 1, format, ap);

    va_end(ap); // ap = NULL

    // 每次打开太麻烦
    // umask(0);
    // int fd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
    // assert(fd >= 0);

    FILE *out = (level == FATAL) ? stderr : stdout;
    fprintf(out, "%s | %u | %s | %s\n",
            log_level[level],
            (unsigned int)time(nullptr),
            name == nullptr ? "unknow" : name,
            logInfo);

    fflush(out); // 将C缓冲区中的数据刷新到OS
    fsync(fileno(out));   // 将OS中的数据尽快刷盘

    // close(fd);
    // char *s = format;
    // while(s){
    //     case '%':
    //         if(*(s+1) == 'd')  int x = va_arg(ap, int);
    //     break;
    // }
}
```


