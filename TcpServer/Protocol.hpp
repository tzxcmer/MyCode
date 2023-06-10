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
    //确认len是否有效
    assert(len);
    size_t pos = in.find(CRLF);
    if(pos == std::string::npos)
    {
        return "";
    }

    //提取长度
    std::string inLen = in.substr(0,pos);
    int intLen = atoi(inLen.c_str());

    //确认有效载荷
    int surplus = in.size() - 2*CRLF_LEN - pos;
    if(surplus < intLen)
    {
        return "";
    }

    //确认报文结构是否完整
    std::string package = in.substr(pos+CRLF_LEN,intLen);
    *len = intLen;

    //将当前报文congin中移除
    int removeLen = inLen.size() + package.size() + CRLF_LEN * 2;
    in.erase(0,removeLen);

    return package;

}

//将序列化的字符串添加长度
std::string encode(std::string &in,uint32_t len)
{
    std::string encodein = std::to_string(len);
    encodein += CRLF;
    encodein += in;
    encodein += CRLF;

    return encodein;
}


class Request{
public:
    Request(){}
    ~Request(){}

    void serialize(std::string *out)
    {
#ifdef MY_SELF
    std::string xstr = std::to_string(x_);
    std::string ystr = std::to_string(y_);
    
    *out = xstr;
    *out += SPACE;
    *out += op_;
    *out += SPACE;
    *out += ystr;

#else
    Json::Value root;
    root["x"] = x_;
    root["y"] = y_;
    root["op"] = op_;

    Json::FastWriter fw;
    *out = fw.write(root);
#endif
    }

    bool deserialize(std::string& in)
    {
#ifdef MY_SELF
        std::size_t spaceOne = in.find(SPACE);
        if(std::string::npos == spaceOne)
        {
            return false;
        }
        std::size_t spaceTwo = in.rfind(SPACE);
        if(std::string::npos == spaceTwo)
        {
            return false;
        }

        std::string dataOne = in.substr(0,spaceOne);
        std::string dataTwo = in.substr(spaceTwo + SPACE_LEN);
        std::string oper = in.substr(spaceOne+SPACE_LEN,spaceTwo-(spaceOne + SPACE_LEN));
        if(oper.size() != 1)
        {
            return false;
        }

        x_ = atoi(dataOne.c_str());
        y_ = atoi(dataTwo.c_str());
        op_ = oper[0];

#else


#endif

        return true;
    }


public:
    int x_;
    int y_;
    char op_;
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