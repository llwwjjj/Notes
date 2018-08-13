#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;
#define PORT 3666//定义用户连接端口
#define MAXDATASIZE 100//允许等待连接数
int main(){
        int sockfd,numbytes;
        char buf[MAXDATASIZE];
        struct sockaddr_in their_addr;//connector's address information
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(sockfd == -1){
                cerr<<"socket error"<<endl;
                return -1;
        }
        cout<<"创建套接字成功"<<endl;

        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(PORT);
        their_addr.sin_addr.s_addr = inet_addr("172.27.138.8");
        bzero(&(their_addr.sin_zero),8);

        if(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1){
                cerr<<"connect error"<<endl;
                return -1;
        }
        cout<<"连接成功"<<endl;

        if((numbytes = recv(sockfd,buf,MAXDATASIZE,0)) == -1){
                cerr<<"recv error"<<endl;
                return -1;
        }
        cout<<buf<<endl;
        close(sockfd);
        return 0;
}