#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

using namespace std;

int main(int argc,char *argv[]){
	if(argc != 2){
		cout<<"Plz Input More"<<endl;
		return -1;
	}

	//socket
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		cout<<"socket error"<<endl;
		return -1;
	}

	struct sockaddr_in addr;
	const char *ip = argv[1];
	inet_aton(ip,&(addr.sin_addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(13579);
	bzero(&(addr.sin_zero),8);


	//connect
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		cout<<"connect error"<<endl;
		return -1;
	}
	cout<<"连接成功"<<endl;

	while(1){
		char message[1024];
		char back_msg[1024];
		//string back_msg;
		cin>>message;
		cout<<message<<endl;

		if(!strcmp(message,"q")||!strcmp(message,"Q"))
			break;
		//int str_len = write(sockfd,message.c_str(),strlen(message.c_str()));
		if(write(sockfd,message,strlen(message)) != strlen(message)){
			cout<<"write error"<<endl;
			return -1;
		}
		//int ret = read (sockfd,back_msg,1024);
		//cout <<ret<< endl;
		//int str_cnt = 0;
		//while(str_cnt < str_len)
			//str_cnt += read(sockfd,&back_msg[str_cnt],1024); 
		cout<<back_msg<<endl;
	}
	close(sockfd);
}