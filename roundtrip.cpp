#include "InetAddress.h"
#include "Socket.h"
#include <thread>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
using namespace std;

struct Message{
	int64_t request; //T1，客户端传过去的时间
	int64_t response;//T2，服务端传回来的时间
}__attribute__((__packed__));

//返回当前时间戳
int64_t now(){
	struct timeval tv = {0,0};
	gettimeofday(&tv,NULL);
	return tv.tv_sec * int64_t(1000000) + tv.tv_usec;
}

//server端启动这个
void runserver(){
	Socket sock(Socket::createUDP());//将socket操作封装成一个类
	sock.bindOrDie(InetAddress(6789));//bind

	while(1){
		Message message = {0,0};
		struct sockaddr theiraddr;
		bzero(&theiraddr,sizeof(theiraddr));
		socklen_t addrLen = sizeof(theiraddr);
		ssize_t nr = recvfrom(sock.fd(),&message,sizeof(message),0,&theiraddr,&addrLen);
		if(nr == sizeof(message)){
			message.response = now();//如果成功收到数据包则设置T2
			ssize_t nw = sendto(sock.fd(),&message,sizeof(message),0,&theiraddr,addrLen);
			if(nw != sizeof(message))
				cout<<"send error"<<endl;
		}
		else
			cout<<"recv error"<<endl;
	}
}

//客户端启动这个
void runclient(const char* server_hostname){
	Socket sock(Socket::createUDP());
	InetAddress serveraddr(6789);
	if(!InetAddress::resolve(server_hostname,&serveraddr)){
		cout<<"resolve error"<<endl;
		return;
	}
	if(sock.connect(serveraddr) != 0){
		cout <<"connnect error"<<endl;
		return;
	}
	//thread thr(fun())传一个函数进去实例化一个线程对象thr，并执行
	thread thr([&sock](){
		while(1){
			Message message = {0,0};
			message.request = now();
			int nw = sock.write(&message,sizeof(message));
			if(nw != sizeof(message))
				cout << "write error"<<endl;
			usleep(200*1000);
		}
	});
	while(1){
		Message message = {0,0};
		int nr = sock.read(&message,sizeof(message));
		if(nr == sizeof(message)){
			int64_t back = now();
			int64_t mine = (back + message.request) / 2;
			cout<<"now "<<back<<" roundtrip "<<back - sock.request <<" clock error "<<message.response - mine<<endl;
		}
		else
			cout<<"read error "<<endl;
	}
};
int main(int argc,char *argv[]){
	if(argc < 2){
		cout<<"plz input more"<<endl;
		return -1;
	} 
	if(strcmp(argv[1],"-s") == 0)
		runserver();
	else
		runclient(argv[1]);
	return 0;
}
