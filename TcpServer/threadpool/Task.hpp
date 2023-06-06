#pragma once

#include <iostream>
#include <string>
#include <functional>
#include "log.hpp"

// typedef std::function<void (int , const std::string &, const uint16_t &)> func_t;
using func_t = std::function<void (int , const std::string &, const uint16_t &, const std::string &)>;

class Task
{
public:
    Task(){}
    Task(int sock, const std::string ip, uint16_t port, func_t func)
    : _sock(sock), _ip(ip), _port(port), _func(func)
    {}
    void operator ()(const std::string &name)
    {
        _func(_sock, _ip, _port, name);
    }
public:
    int _sock;
    std::string _ip;
    uint16_t _port;
    // int type;
    func_t _func;
};