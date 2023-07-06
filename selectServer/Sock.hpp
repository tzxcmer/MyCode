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
    static const int gbacklock = 20;

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

    }

    static void Listen(int socket)
    {

    }

    static int Accept(int socket, std::string *clientip, uint16_t *clientport)
    {

    }

};