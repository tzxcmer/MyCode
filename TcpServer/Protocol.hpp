#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <jsoncpp/json/json.h>
#include "until.hpp"

// 我们要在这里进行我们自己的协议定制!
// 网络版本的计算器

#define CRLF "\r\n"
#define CRLF_LEN strlen(CRLF) // 坑：sizeof(CRLF)
#define SPACE " "
#define SPACE_LEN strlen(SPACE)

#define OPS "+-*/%"

std::string decode(std::string &in,uint32_t *len)
{

}


std::string encode(std::string &in,uint32_t *len)
{

}


class Request{

};


class Response
{
private:
    /* data */
public:
    Response(/* args */);
    ~Response();
};


bool makeRequest(const std::string &str,Request *req)
{

}