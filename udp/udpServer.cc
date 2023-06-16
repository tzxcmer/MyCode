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

    }

    void checkOnlineUser()
    {

    }

    void messageRoute()
    {

    }
};



int main(int argc, char* argv[])
{

    return 0;
}
