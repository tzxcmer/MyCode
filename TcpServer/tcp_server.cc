#include"until.hpp"
#include"Task.hpp"
#include"daemonize.hpp"
#include"ThreadPool.hpp"


#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<assert.h>


void netCal(int sock, const std::string &clientIp, uint16_t clientPort)
{
    assert(sock >= 0);
    assert(!clientIp.empty());
    assert(clientPort >= 1024);

    std::string inbuffer;
    while(true)
    {
        
    }
}

class SeverTcp;

class ThreadData{
public:
    uint16_t clientPort_;
    std::string clientIp_;
    int sock_;
    SeverTcp *this_;
public:
    ThreadData(uint16_t port,std::string ip,int sock,SeverTcp *ts):
    clientPort_(port),clientIp_(ip),sock_(sock),this_(ts)
    {}
};

class ServerTcp{
public:
    ServerTcp(uint16_t port,const std::string &ip=""):
    listenSock_(-1),port_(port),ip_(ip),tp_(nullptr)
    {
        quit_ = false;
    }

    ~ServerTcp()
    {
        if(listenSock_ >= 0)
            close(listenSock_);
    }

public:
    void init()
    {
        listenSock_ = socket(PF_INET,SOCK_STREAM,0);
        if(listenSock_ < 0)
        {
            logMessage(FATAL, "socket: %s", strerror(errno));
            exit(SOCKET_ERR);
        }
        logMessage(DEBUG, "socket: %s, %d", strerror(errno), listenSock_);
        
        struct sockaddr_in local;
        memset(&local,0,sizeof local);
        local.sin_family = PF_INET;
        local.sin_port = htons(port_);
        ip_.empty() ? (local.sin_addr.s_addr = INADDR_ANY) : (inet_aton(ip_.c_str(), &local.sin_addr));

        //本地socket信息，写入sock_对应的内核区域
        if (bind(listenSock_, (const struct sockaddr *)&local, sizeof local) < 0)
        {
            logMessage(FATAL, "bind: %s", strerror(errno));
            exit(BIND_ERR);
        }
        logMessage(DEBUG, "bind: %s, %d", strerror(errno), listenSock_);

        // 监听socket
        if (listen(listenSock_, 5 /*后面再说*/) < 0)
        {
            logMessage(FATAL, "listen: %s", strerror(errno));
            exit(LISTEN_ERR);
        }
        logMessage(DEBUG, "listen: %s, %d", strerror(errno), listenSock_);

        // 加载线程池
        tp_ = ThreadPool<Task>::getInstance();
    }

    void loop()
    {
        tp_->start();
        logMessage(DEBUG,"thread pool staart sucess,thread num:%d",tp_->threadNum());

        while (!quit_)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof peer;
            //给用户提供sock服务
            int serviceSock = accept(listenSock_,(struct sockaddr*)&peer,&len);
            if(quit_)
            {
                break;
            }

            if(serviceSock < 0)
            {
                logMessage(WARINING,"accept:%s[%d]",strerror(errno),serviceSock);
                continue;
            }

            //获取客户端基本信息(端口和ip)
            uint16_t peerPort = ntohs(peer.sin_port);
            std::string peerIp = inet_ntoa(peer.sin_addr);

            logMessage(DEBUG,"accept: %s | %s[%d], socket fd:%d",
            strerror(errno),peerIp.c_str(),peerPort,serviceSock);

            Task t(serviceSock, peerIp, peerPort, netCal);
            tp_->push(t);
        }
    }

    bool quitServer()
    {
        quit_ = true;
        return true;
    }

private:
    int listenSock_;
    uint16_t port_;
    std::string ip_;
    ThreadPool<Task> *tp_;
    bool quit_;
};