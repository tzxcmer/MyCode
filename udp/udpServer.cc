#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <unordered_map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include"Log.hpp"

static void Usage(const std::string porc)
{
    std::cout << "Usage:\n\t" << porc << " port [ip]" << std::endl;
}

class UdpServer
{
private:
    /* data */
    // 服务器必须得有端口号信息
    uint16_t port_;
    // 服务器必须得有ip地址
    std::string ip_;
    // 服务器的socket fd信息
    int sockfd_;
    // onlineuser
    std::unordered_map<std::string, struct sockaddr_in> users;
public:
    UdpServer(int port, std::string ip = ""):
    port_((uint16_t)port),ip_(ip),sockfd_(-1)
    {

    }
    ~UdpServer()
    {
    }
public:
    void init()
    {
        //创建套接字
        sockfd_ = socket(AF_INET,SOCK_STREAM,0);
        if (sockfd_ < 0)
        {
            logMessage(FATAL, "socket:%s:%d", strerror(errno), sockfd_);
            exit(1);
        }
        logMessage(DEBUG, "socket create success: %d", sockfd_);

        struct sockaddr_in local;  
        bzero(&local,sizeof local);

        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        local.sin_addr.s_addr = ip_.empty() ? htonl(INADDR_ANY) : inet_addr(ip_.c_str());

        //bind网络信息
        if (bind(sockfd_, (const struct sockaddr *)&local, sizeof(local)) == -1)
        {
            logMessage(FATAL, "bind: %s:%d", strerror(errno), sockfd_);
            exit(2);
        }
        logMessage(DEBUG, "socket bind success: %d", sockfd_);
    }

    void start()
    {
        char inbuffer[1024];
        char outbuffer[1024];

        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof peer;

            ssize_t s = recvfrom(sockfd_,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr*)&peer,&len);
            if (s > 0)
            {
                inbuffer[s] = 0; //当做字符串
            }
            else if (s == -1)
            {
                logMessage(WARINING, "recvfrom: %s:%d", strerror(errno), sockfd_);
                continue;
            }

            std::string peerIp = inet_ntoa(peer.sin_addr);
            uint32_t peerPort = ntohs(peer.sin_port);
            checkOnlineUser(peerIp, peerPort, peer); //如果存在，什么都不做，如果不存在，就添加

            messageRoute(peerIp, peerPort,inbuffer); //消息路由
        }
    }

    void checkOnlineUser(std::string &ip, uint32_t port, struct sockaddr_in &peer)
    {
        std::string key = ip;
        key += ":";
        key += std::to_string(port);
        auto iter = users.find(key);
        if(iter == users.end())
        {
            users.insert({key,peer});
        }
    }

    void messageRoute(std::string ip, uint32_t port, std::string info)
    {
         std::string message = "[";
        message += ip;
        message += ":";
        message += std::to_string(port);
        message += "]# ";
        message += info;

        for(auto& user : users)
        {
            sendto(sockfd_,message.c_str(),message.size(),0,
            (const sockaddr*)&(user.second),sizeof user.second);
        }
    }
};



int main(int argc, char* argv[])
{

    return 0;
}
