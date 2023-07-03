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

    static int socket()
    {

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