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
#define MYPORT 3666//定义用户连接端口
#define BACKLOG 10//允许等待连接数

using namespace std;

int main(){
        int sockfd,new_fd;//描述符
        struct sockaddr_in my_addr;//存储本地端口和IP信息
        struct sockaddr_in their_addr;//存储客户端端口和IP信息
        //----------------------socket-------------------------
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(sockfd == -1){
                cerr<<"socket error"<<endl;
                return -1;
        }
        cout<<"创建套接字成功"<<endl;
        //-----------------------bind--------------------------
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(MYPORT);
        my_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(my_addr.sin_zero),8);
        if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr)) == -1){
                cerr<<"bind error"<<endl;
                return -1;
        }
        cout<<"绑定套接字成功"<<endl;
        //----------------------listen-------------------------
        if(listen(sockfd,BACKLOG) == -1){
                cerr<<"listen error"<<endl;
                return -1;
        }
        cout<<"服务端启动成功，开始监听"<<endl;
        while(1){
        //----------------------accept-------------------------
                socklen_t sin_size = sizeof(struct sockaddr_in);
                new_fd = accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);//如果没有客户连接过来，则保持阻塞
                if(new_fd == -1){
                        cerr<<"accept error"<<endl;
                        break;
                }
                //成功连接
                cout<<"与客户端建立连接，客户端IP为："<<inet_ntoa(their_addr.sin_addr)<<endl;
        //----------------------send---------------------------
                if(send(new_fd,"hello\n",6,0) == -1){
                        cerr<<"send"<<endl;
                        break;
                }
                close(new_fd);
        }
        close(sockfd);
        return 0;
}