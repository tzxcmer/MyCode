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


class Sock{
public:
    static const int gbacklog = 20;

    static int  Socket()
    {
        int listenSock = socket(PF_INET, SOCK_STREAM, 0);
        if (listenSock < 0)
        {
            exit(1);
        }
        int opt = 1;
        setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        return listenSock;
    }

    static void Bind(int socket,uint16_t port)
    {
        struct sockaddr_in local; // 用户栈
        memset(&local, 0, sizeof local);
        local.sin_family = PF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        // 2.2 本地socket信息，写入sock_对应的内核区域
        if (bind(socket, (const struct sockaddr *)&local, sizeof local) < 0)
        {
            exit(2);
        }
    }

    static void Listen(int socket)
    {
        if (listen(socket, gbacklog) < 0)
        {
            exit(3);
        }
    }

    static int Accept(int socket, std::string *clientip, uint16_t *clientport)
    {

    }

};