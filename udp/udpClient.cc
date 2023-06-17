#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <unistd.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

struct sockaddr_in server;

static void Usage(std::string name)
{
    std::cout << "Usage:\n\t" << name << " server_ip server_port" << std::endl;
}

void *recverAndPrint(void *args)
{
    while(true)
    {
        int sockfd = *(int*)args;
        char buff[1024];
        struct sockaddr_in temp;
        socklen_t len = sizeof temp;
        ssize_t s = recvfrom(sockfd,buff,sizeof(buff)-1,0,(struct sockaddr*)&temp,&len);
        if (s > 0)
        {
            buff[s] = 0;
            std::cout << "server echo# " << buff << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    return 0;
}