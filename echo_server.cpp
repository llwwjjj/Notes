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
	//socket
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd < 0){
		cout<<"socket error"<<endl;
		return -1;
	}
	struct sockaddr_in my_addr,their_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(13579);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(my_addr.sin_zero),8);
	//bind
	if(bind(listenfd,(struct sockaddr*)&my_addr,sizeof(my_addr)) == -1){
		cout<<"bind error"<<endl;
		return -1;
	}
	//listen
	if(listen(listenfd,5) == -1){
		cout<<"listen error"<<endl;
		return -1;
	}

	for(int i = 0;i < 5;++i){
	//accept
		cout<<"等待连接"<<endl;
		socklen_t addrlen = sizeof(struct sockaddr_in);
		int new_fd = accept(listenfd,(struct sockaddr*)&their_addr,&addrlen);
		if(new_fd == -1){
			cout<<"accept error"<<endl;
			break;
		}
		cout<<"成功连接"<<endl;
	//read and write
		const int buf_size = 1024;
		int str_len;//从read返回，得知收了多少字节的数据，然后再发回去
		char message[buf_size];
		while(str_len = read(new_fd,message,buf_size) != 0){
			if(write(new_fd,message,str_len) != str_len){
				cout<<"write error"<<endl;
				break;
			}
			str_len = 0;
		}
		//str_len = read(new_fd,message,buf_size);
		//cout<<str_len<<endl;
		//write(new_fd,message,str_len);
	//close
		close(new_fd);
	}
	close(listenfd);
	return 0;
}