#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <cerrno>
#include <cassert>

#define CRLF "\r\n"
#define SPACE " "
#define SPACE_LEN strlen(SPACE)
#define HOME_PAGE "index.html"
#define ROOT_PATH "wwwroot"

using namespace std;


std::string getPath(std::string http_request)
{
    std::size_t pos = http_request.find(CRLF);
    if(pos == std::string::npos) 
        return "";
    std::string request_line = http_request.substr(0, pos);
    
    std::size_t first = request_line.find(SPACE);
    if(pos == std::string::npos) 
        return "";
    std::size_t second = request_line.rfind(SPACE);
    if(pos == std::string::npos) 
        return "";

    std::string path = request_line.substr(first+SPACE_LEN, second - (first+SPACE_LEN));
    if(path.size() == 1 && path[0] == '/')
        path += HOME_PAGE;
    
    return path;
}

std::string readFile(const std::string &recource)
{
    std::ifstream in(recource,std::ifstream::binary);
    if(!in.is_open())
        return "404";
    
    std::string content;
    std::string line;
    while (std::getline(in,line))
    {
        content += line;
    }

    in.close();

    return content;
    
}

void handlerHttpRequest(int sock)
{
    char buffer[10240];
    ssize_t s = read(sock, buffer, sizeof buffer);
    if(s > 0) cout << buffer;
    std::string path = getPath(buffer);
    
    std::string recource = ROOT_PATH;
    recource += path;
    std::cout << recource << std::endl;

    std::string html = readFile(recource);
    std::size_t pos = recource.rfind(".");
    std::string suffix = recource.substr(pos);
    cout << suffix << endl;

    //开始响应
    std::string response;
    response = "HTTP/1.0 200 OK\r\n";
    if(suffix == ".jpg") response += "Content-Type: image/jpeg\r\n";
    else response += "Content-Type: text/html\r\n";
    response += ("Content-Length: " + std::to_string(html.size()) + "\r\n");
    response += "\r\n";
    response += html;

    send(sock, response.c_str(), response.size(), 0);                           
}


class ServerTcp
{
public:
    ServerTcp(uint16_t port, const std::string &ip = "")
        : port_(port),
          ip_(ip),
          listenSock_(-1)
    {
        quit_ = false;
    }
    ~ServerTcp()
    {
        if (listenSock_ >= 0)
            close(listenSock_);
    }

public:
    void init()
    {
        // 1. 创建socket
        listenSock_ = socket(PF_INET, SOCK_STREAM, 0);
        if (listenSock_ < 0)
        {
            exit(1);
        }
        // 2. bind绑定
        // 2.1 填充服务器信息
        struct sockaddr_in local; // 用户栈
        memset(&local, 0, sizeof local);
        local.sin_family = PF_INET;
        local.sin_port = htons(port_);
        ip_.empty() ? (local.sin_addr.s_addr = INADDR_ANY) : (inet_aton(ip_.c_str(), &local.sin_addr));
        // 2.2 本地socket信息，写入sock_对应的内核区域
        if (bind(listenSock_, (const struct sockaddr *)&local, sizeof local) < 0)
        {
            exit(2);
        }

        // 3. 监听socket，为何要监听呢？tcp是面向连接的！
        if (listen(listenSock_, 5 /*后面再说*/) < 0)
        {
            exit(3);
        }
        // 运行别人来连接你了
    }
    void loop()
    {
        signal(SIGCHLD, SIG_IGN); // only Linux
        while (!quit_)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);

            int serviceSock = accept(listenSock_, (struct sockaddr *)&peer, &len);
            if (quit_)
                break;
            if (serviceSock < 0)
            {
                // 获取链接失败
                cerr << "accept error ...." << endl;
                continue;
            }
            // 5.1 v1 版本 -- 多进程版本 -- 父进程打开的文件会被子进程继承吗？会的
            pid_t id = fork();
            assert(id != -1);
            if(id == 0)
            {
                close(listenSock_); //建议
                if(fork() > 0) exit(0);
                //孙子进程
                handlerHttpRequest(serviceSock);
                exit(0); // 进入僵尸
            }
            close(serviceSock);
            wait(nullptr);
        }
    }

    bool quitServer()
    {
        quit_ = true;
        return true;
    }

private:
    // sock
    int listenSock_;
    // port
    uint16_t port_;
    // ip
    std::string ip_;
    // 安全退出
    bool quit_;
};