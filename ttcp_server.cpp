#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <stdint.h>
#include <memory> 
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
    
using namespace std;
    
struct SessionMessage{
        int32_t number;
        int32_t length;
}__attribute__((__packed__));//共8个字节

struct PayloadMessage{
        int32_t length;
        char data[0];//运行时决定struct长度
};

double now(){//获取当前时间，精确到微秒
        struct timeval tv = {0,0};//列表初始化
        gettimeofday(&tv,NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static int write_n(int sockfd,const void *buf,int length){
        int written = 0;//总共要写长度为length的数据，现在已经写了0
        while(written < length){
                //每次都从已经之前写到的位置开始写
                ssize_t nw = write(sockfd,static_cast<const char*>(buf) + written,length - written);
                if(nw > 0)
                        written += static_cast<int>(nw);
                else if(nw == 0)
                        break;
                else if(errno != EINTR){
                        cerr<<"write error"<<endl;
                        break;
                }
        }
        return written;
}

static int read_n(int sockfd,void *buf ,int length){
        int nread = 0;//已读0
        while(nread < length){
                ssize_t nr = read(sockfd,static_cast<char*>(buf) + nread,length - nread);
                if(nr > 0)
                        nread += static_cast<int>(nr);
                else if(nr == 0)
                        break;
                else if(nr < 0){
                        cerr<<"read error"<<endl;
                        break;
                }
        }
        return nread;
}

//总的来说作用是每次调用都新建一个监听套接字并listen直到连接到来，accept后关闭监听套接字并返回新套接字
static int acceptOrDie(uint16_t port){
	int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	assert(listenfd >= 0);
	int yes = 1;
	//--------------------------------socket--------------------------------
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes))){
		cout<<"setsockopt error"<<endl;
		exit(1);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(addr.sin_zero),8);
	cout<<"创建套接字成功"<<endl;
	//--------------------------------bind----------------------------------
	if(bind(listenfd,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr))){
		cout<<"bind error"<<endl;
		exit(1);
	}
	cout<<"绑定套接字成功"<<endl;
	//-------------------------------listen---------------------------------
	if(listen(listenfd,5)){
		cout<<"listen error"<<endl;
		exit(1);
	}
	cout<<"正在监听套接字"<<endl;
	//-------------------------------accept---------------------------------
	struct sockaddr_in peer_addr;
	bzero(&(peer_addr.sin_zero),8);
	socklen_t addrlen = 0;
	int sockfd = accept(listenfd,reinterpret_cast<struct sockaddr*>(&peer_addr),&addrlen);
	if(sockfd < 0){
		cout<<"accept error"<<endl;
		exit(1);
	}
	close(listenfd);
	cout<<"连接成功，将返回新套接字"<<endl;
	return sockfd;
}
int main(){
	int sockfd = acceptOrDie(12345);
	SessionMessage sessionMessage = {0,0};//列表初始化，用了接受对面传过来的sessionMessage
	//-------------------------------write-----------------------------------
	if(read_n(sockfd,&sessionMessage,sizeof(sessionMessage)) != sizeof(sessionMessage)){//如果没有全部读完就返回错误
		cout<<"read error"<<endl;
		return -1;
	}
	sessionMessage.length = ntohl(sessionMessage.length);//转换成本机字节序
	sessionMessage.number = ntohl(sessionMessage.number);
	cout<<"receive number = "<<sessionMessage.number<<" "<<"receive length = "<<sessionMessage.length<<endl;
	//得知客户端要传多少个字节的PayloadMessage过来
	const int total_len = static_cast<int>(sizeof(int32_t) + sessionMessage.length);
	PayloadMessage *payloadMessage = static_cast<PayloadMessage*>(malloc(total_len));
	assert(payloadMessage);
	for(int i = 0;i < sessionMessage.number;++i){
		//----------------------------read-----------------------------------
		//先传length过来，对比一下之前传过来的length
		if(read_n(sockfd,&(payloadMessage->length),sizeof(payloadMessage->length)) != sizeof(payloadMessage->length)){
			cout<<"read PayloadMessage.length error"<<endl;
			return -1;
		}
		payloadMessage->length = ntohl(payloadMessage->length);
		if(i % 100 ==0)
			cout<<payloadMessage->length<<endl;
		assert(payloadMessage->length == sessionMessage.length);
		//传数据
		if(read_n(sockfd,payloadMessage->data,payloadMessage->length) != payloadMessage->length){
			cout<<"read PayloadMessage.data error"<<endl;
			return -1;
		}
		//-----------------------------write----------------------------------
		int32_t ack = htonl(payloadMessage->length);
		if(write_n(sockfd,&ack,sizeof(ack)) != sizeof(ack)){
			cout<<"write ack error"<<endl;
			return -1;
		}
	}

	free(payloadMessage);
	close(sockfd);
	cout<<"关闭套接字"<<endl;
	return 0;
}

