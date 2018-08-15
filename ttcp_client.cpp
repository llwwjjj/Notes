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
}__attribute__((__packed__));//共8个字节，先传一个SessionMessage过去告诉服务端，之后要传的PayloadMessage是多少个字节

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

int main(int argc,char *argv[]){
	if(argc<2){
		cout<<"plz Input more"<<endl;
		return 1;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	const char *ip = argv[1];
	inet_aton(ip,&(addr.sin_addr));
	addr.sin_port = htons(12345);
	bzero(&(addr.sin_zero),8);
//-----------------------------------socket------------------------------------
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		cout<<"socket error"<<endl;
		return -1;
	}
//----------------------------------connect------------------------------------
	int ret = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	if(ret){
		cout<<"connect error"<<endl;
		return -1;
	}
	printf("connected\n");	

	int length = 8192;//指定一个message的字节长度,一次传8KB
	int number = 1024;//number指定要传多少个PayloadMessage

	double start = now();
	SessionMessage sessionMessage = {0,0};//列表初始化
	sessionMessage.length = htonl(length);//转换成网络字节序
	sessionMessage.number = htonl(number);
//-----------------------------------write--------------------------------------
	if(write_n(sockfd,&sessionMessage,sizeof(sessionMessage)) != sizeof(sessionMessage)){
		cout<<"write error"<<endl;
		return -1;
	}
//-------------------------构造要传的数据payloadmessage---------------------------
	const int total_len = static_cast<int>(sizeof(int32_t) + length);
    PayloadMessage *payloadMessage = static_cast<PayloadMessage*>(malloc(total_len));
    assert(payloadMessage);
    //赋值
    payloadMessage->length = htonl(length);
    for(int i = 0;i < length; ++i)
    	payloadMessage->data[i] = "0123456789ABCDEF"[i%16];
   	double total_mb = 1.0 * length * number / 1024 / 1024;
   	cout<<"共有 "<<total_mb<<" MB 需要传输"<<endl;
   	for(int i = 0;i < number;++i){
//----------------------------------write----------------------------------------
   		if(write_n(sockfd,payloadMessage,total_len) != total_len){
   		//if(write_n(sockfd,&(payloadMessage->length),sizeof(payloadMessage->length)) != sizeof(payloadMessage->length)){
   			cout<<"write PayloadMessage error"<<endl;
   			return -1;
   		}
//----------------------------------read-----------------------------------------
   		int32_t ack;
   		if(read_n(sockfd,&ack,sizeof(ack)) != sizeof(ack)){
   			cout<<"read ack error"<<endl;
   			return -1;
   		}
   		ack = ntohl(ack);

   		assert(ack == length);
   	}

   	free(payloadMessage);
   	close(sockfd);
   	double use_time = now() - start;
   	cout<< total_mb / use_time <<endl;
	return 0;
}

