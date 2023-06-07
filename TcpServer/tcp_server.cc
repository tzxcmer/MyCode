#include"until.hpp"
#include"Task.hpp"
#include"daemonize.hpp"
#include"ThreadPool.hpp"


#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>

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
        quit = false;
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

    }
private:
    int listenSock_;
    uint16_t port_;
    std::string ip_;
    ThreadPool<Task> *tp_;
    bool quit;
};