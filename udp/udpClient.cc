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
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd > 0);

    bzero(&server, sizeof server);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    pthread_t t;
    pthread_create(&t,nullptr,recverAndPrint,(void*)&sockfd);
    
    std::string buffer;

    while(true)
    {
        std::cerr << "Please Enter# ";
        std::getline(std::cin, buffer);
        sendto(sockfd,buffer.c_str(),buffer.size(),0,
        (const struct sockaddr *)&server,sizeof server);
    }

    return 0;
}