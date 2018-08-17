# Notes
- 网络编程相关随笔，学习记录
-------------
目录
-------------
* __ attribute __
  - GNU C 中关键字__attribute__可以设置函数属性，变量属性和类型属性。使用方法__attribute__((__参数__));
也可以对struct或union进行属性设置，使用方法 struct xxx{int x;}__ attribute __((__参数__));
参数可以为：aligned，packed，transparent_union，unused，may_alias.
    - aligned: 设定一个指定大小的对齐格式，而且必须是2的指数倍，如"
    ```cpp
    struct xxx {
      int x[3];
    };__attribute__((__aligned(8)));
    ```
    强制设定struct的变量在分配空间时采用8字节对齐方式，但如果超过连接器最大支持字节数是无济于事的,小于struct内最大类型也无效。测试：
    ```cpp
    #include<iostream>
    using namespace std;

    struct xxx {
        int a;
        char b;
        short c;
    }__attribute__((aligned(4))) xx;

    struct yyy {
        char a;
        int b;
        short c;
    }__attribute__((aligned(4))) yy;

    struct zzz {
        char a;
        short b;
        short c;
    }__attribute__((aligned(4))) zz;

    int main(){
        cout<<sizeof(xx)<<endl;//得到8，因为4+2+1按4对齐变成8
        cout<<sizeof(yy)<<endl;//得到12，因为(1->4)+4+2按4对齐变成12
        cout<<sizeof(zz)<<endl;//得到8，因为(1+2=3->4)+2按4对齐变成8
        return 0;
    }
    ```
    - packed: aligned属性使被设置的对象占用更多的空间，相反的，packed可以减小对象占用的空间。测试：
    ```cpp
    struct aaa {
      char a;
      short b;
      int c;
    }__attribute__((__packed__)) aa;
    cout<<sizeof(aa)<<endl;//得到7，因为1+2+4=7
    ```
    - __attribute__改变函数属性，如：
    ```cpp
    #include<iostream>
    using namespace std;

    static __attribute__((constructor)) void before(){
      cout<<"hello ";//在main函数之前执行
    }

    static __attribute__((destructor)) void after(){
      cout<<"world!"<<endl;//在main函数之后执行
    }

    int main(){
      return 0;
    }//编译运行，显示hello world
    ```
* Socket：
  - 什么是socket？
    - socket是使用标准Unix文件描述符和其他程序通讯的方式。Unix程序在执行任何形式的I/O的时候，程序都是在读或者写一个文件描述符，而一个文件描述符是和一个打开的文件相关联的整数。而这个文件可能是一个网络连接，FIFO，管道等。所以，你想和Internet上别的程序通讯的时候，你将要使用到文件描述符。如何获得网络通讯的文件描述符呢?利用系统调用socket(),它返回套接字描述符，然后再通过send()和recv()调用。
  - 需要了解的两种internet套接字类型： 
    - 流格式套接字： 是可靠的双向通讯的数据流，如果你向流套接字按顺序输出“1,2”，那么他们将按顺序“1,2”到达另一边，他们是无错误传递的，有自己的错误控制，使用传输控制协议 TCP。(SOCK_STREAM)
    - 数据包格式套接字： 也叫无连接套接字，如果你发送一个数据报，他可能会到达，可能会次序颠倒；如果他到达了，那么在这个包的内部是无错误的，使用用户数据报协议 UDP。(SOCK_DGRAM)
    - 为什么数据报格式套接字是无连接的？主要是因为他不像流格式套接字那样维持一个连接，你只要建立一个包，构造一个有目标信息的IP头，然后发出去，无需连接。收到者必须发回一个ACK包，如果在一定时间发送方没有收到ACK，将重新发送，直到得到ACK。
  - 七层协议：应用层(telnet,ftp等),表示层，会话层，传输层(TCP,UDP)，网络层(IP和路由)，数据链路层，物理层。
  - 结构体sockaddr，in_addr和sockaddr_in：
    - socket描述符的类型是int
    - 在头文件 sys/socket.h，netinet/in.h和arpa/inet.h下
    - sockaddr: sockaddr是通用的socket地址，此数据结构用做bind，connect，recvfrom，sendto等函数的参数，指明地址信息。但一般编程中并不直接对此数据结构操作，而是使用另一个与sockaddr等价的数据结构sockaddr_in。
    ```cpp
    struct sockaddr{
      unsigned short sa_family;
      char sa_data[14];
    };
    ```
       --其中sa_family是地址家族，一般都是AF_xxx的形式，代表tcp/ip协议族。
       --sa_data是14字节协议地址。
    - sockaddr_in: 
    ```cpp
    struct sockaddr_in{
      short int sin_family;//地址族，AF_xxx，在socket编程中只能是AF_INET
      unsigned short int sin_port;//端口号，2字节
      struct in_addr sin_addr;//存储IP地址，4字节
      unsigned char sin_zero[8];//8字节，实际上没什么用，是为了和sockaddr保持一样的长度
    };
    ```
       --使用sockaddr_in可以轻松处理套接字地址的基本元素。sin_zero应该使用函数bzero()或menset()来全部置零。sockaddr和sockaddr_in可以互相转换，而且指向这两个结构体的指针也可以互相代替。同时，sin_family和sa_family需一致。最后，sin_port和sin_addr必须是网络字节顺序
     - in_addr: 就是32位IP地址,4个字节(怎么让sin_addr按照网络字节顺序？)
     ```cpp
     struct in_addr {
      unsigned long s_addr;//存储4字节的IP地址(网络字节顺序)
     };
     ```
      --存储4字节的IP地址(网络字节顺序)
  - 本机转换：
    - htons()--"Host to Network Short"
    - htonl()--"Host to Network Long"
    - ntohs()--"Network to Host Short"
    - ntohl()--"Network to Host Long"
    - 为什么在sockaddr_in中，sin_addr和sin_port需要转换成网络字节顺序，而sin_family不需要呢？因为sin_addr和sin_port封装在包的IP和UDP层。因此他们必须是网络字节顺序。但是sin_family域只是被内核使用来决定在数据结构中包含什么类型的地址，所以他必须是本机字节顺序。同时，sin_family没有发送到网络上，所以是本机字节顺序。
  - 处理IP地址：
    - 用函数inet_addr()可以将IP地址从点格式转换成无符号长整数型。使用方法：\
      struct sockaddr_ina; \
      ina.sin_addr = inet_addr("192.168.0.1");\
      inet_addr()返回的地址已经是网络字节格式，所以没必要再使用htonl()
    - 函数inet_ntoa()可以将一个sin_addr转换成点数格式
  - socket()函数：
    - 需要头文件sys/types.h和sys/socket.h
    - 根据地址族，数据类型和协议来分配一个套接字及其所用资源
    - int socket(int domain,int type,int protocol);
    - domain应该设置成AF_INET，可以传inta.sin_family
    - type告诉内核是SOCK_STREAM还是SOCK_DGRAM类型
    - protocol用来指定socket所使用的传输协议编号，其中IPPROTO_TCP为6，IPPROTO_IP为0，IPPROTO_UDP为17
    - socket返回以后在系统调用中可能用到的socket描述符，在错误时返回-1.全局变量errno中存储返回的错误值。
   - bind()函数：
     - 需要头文件sys/types.h和sys/socket.h
     - 一旦有了一个套接字，可能就需要将套接字和设备上的端口关联起来，如果你想用listen()来监听一定端口的数据这步是必须的，如果只想用connect()，那么这个步骤没有必要。
     - int bind(int sockfd,struct sockaddr *my_addr,int addrlen);
     - sockfd是调用socket()返回的文件描述符
     - my_addr是指向sockaddr的指针，一般我们需要传ina,也就是sockaddr_in,所以一般传(const struct sockaddr*)&ina，他保存自己的地址(端口和IP信息)
     - addrlen设置为sizeof(struct sockaddr_in)也就是16字节
     - bind成功返回0；出错返回－1，相应地设定全局变量errno。
   - socket() + bind()实例:
     ```cpp
     int main(){
       int serverfd = -1;
       struct sockaddr_in ina;

       ina.sin_family = AF_INET;
       ina.sin_port = htons(8183);//将整型变量转换成网络字节顺序，TCP中规定好的一种数据表示格式，大端
       ina.sin_addr.s_addr = inet_addr("172.27.138.8");//转换成32位IP地址，也就是8位十六进制数
    
       serverfd = socket(ina.sin_family,SOCK_STREAM,IPPROTO_TCP); 
       if(serverfd < 0){//socket错误检查
        cout<<"创建server的socket套接字失败"<<endl;
        return -1;
       }
 
       bzero(&(ina.sin_zero), 8);

       int ret = bind(serverfd,(const struct sockaddr*)&ina,sizeof(ina));
       if(ret < 0){
        cout<<"绑定本地地址、端口失败"<<endl;
        return -1;
       }
       return 0;
      }
     ```
     - 其中bzero在头文件string.h中，用于置零
     - 也可以让ina.sin_port = 0;意思是随机选择一个没有使用的端口
     - 也可以让ina.sin_port = INADDR_ANY;意思是使用自己的IP地址
     - 不要采用小于1024的端口号，因为所有小于1024的端口都被系统保留
     - 如果直接使用connect()来进行远程通讯，就不需要关心本地端口号，因为它会检查套接字是否绑定端口，如果没有，它会自己绑定一个没有使用的本地端口
     - 不使用bind()意味着不在乎本地端口号，只关心我要去哪。
   - connect()函数
     - 需要头文件sys/types.h和sys/socket.h
     - 用于连接到远程主机，绑定目的地端口和IP
     - int connect(int sockfd,struct sockaddr* serv_addr,int addrlen);
     - sockfd是调用socket()返回的文件描述符,如果之前没有bind过就自己绑定一个本地端口，总之sockfd已经对应了本地IP和端口
     - serv_addr保存着目的地的端口和IP
     - addrlen设置为sizeof(struct sockaddr_in)也就是16字节
     - connect()成功时返回0，错误返回-1，相应地设定全局变量errno
   - listen()函数
     - 需要头文件sys/types.h和sys/socket.h
     - 监听套接字
     - int listen(int sockfd,int backlog);
     - sockfd是调用socket()返回的文件描述符
     - backlog是在进入队列中允许的连接数目，进入的连接是在队列中一直等待直到你accept连接，大多数系统允许数目是20，也有5或10
     - listen()成功返回0，错误返回-1，相应地设定全局变量errno
     - 1.socket()创建套接字 2.bind()将套接字与本地IP和端口关联 3.listen监听套接字
   - accept()函数
     - 需要头文件sys/types.h和sys/socket.h
     - accept可以从等待接受的队列中接受一个连接请求，它将返回一个新的套接字，这样你就有两个套接字了，一个还在监听端口，新的套接字在准备发送send()和接收recv()数据。
     - int accept(int sockfd,struct sockaddr *addr,int *addrlen);
     - sockfd是要接受的连接描述符，和listen的一样
     - addr是一个指向局部sockaddr的指针，这是接入的信息要传入的地方
     - addrlen是一个局部的整型变量，
     - 错误时返回-1，成功时返回新的套接字
     - 在系统调用中send()和recv()都应该使用新的套接字描述符new_fd,如果你只想让一个连接进来，那么可以使用close()去关闭原来的文件描述符sockfd来避免同一个端口更多的连接
   - send()和recv()函数：
     - 需要头文件sys/types.h和sys/socket.h
     - 这两个函数用于流式套接字或者数据报套接字的通讯
     - int send(int sockfd,const void *msg,int len,int flags);
     - sockfd是你想要发送数据的套接字描述符，accep()返回的
     - msg是指向你想要发送的数据的指针
     - len是你要发送的数据的长度
     - flags设置为0就可以了
     - ！！send()返回实际发送的数据的字节数，他可能小于要求发送的数目！！它只是发送他可能发送的数据，如果返回的数据和len不匹配，你就应该发送其他数据，如果你发的包很小(小于大约1K)，他可能处理让数据一次发送完
     - 错误返回-1，并设置errno
     - int recv(int sockfd,void *buf,int len,unsigned int flags);
     - sockfd是要读的套接字描述符
     - buf是要读的信息的缓冲
     - len是缓冲的最大长度
     - flags可以设为0
     - 错误返回-1，并设置errno
   - sendto()和recvfrom()函数：
     - 无连接传数据，需要包含目的地的IP和端口信息
     - 如果你用connect()连接一个数据报套接字，你可以简单地调用send()和recv()来满足你的要求，这个时候依然是数据报套接字，依然是用UDP，系统套接字接口会为你自动加上目标和源的信息。
   - close()和shutdown()函数：
     - 需要头文件sys/types.h和sys/socket.h
     - 在send()和recv()之后，如果需要关闭你的套接字描述符，可以使用close()
     - int close(int sockfd);
     - 它将防止套接字上更多的数据的读写。任何在另一端读写套接字的企图都将返回错误信息。如果你想在如何关闭套接字上多一点控制，可以使用shutdown(),他允许你将一定方向的通讯或者双向通讯关闭
     - int shutdown(int sockfd,int how);
     - how = 0 ->不允许接受 1->不允许发送 2->不允许发送和接受，和close一样
     - shutdown()成功返回0，失败返回-1，如果无连接的数据报套接字中使用shutdown()，那么只不过是让send()和recv()不能使用。
  - getpeername()函数：
     - 需要头文件sys/types.h和sys/socket.h
     - 告诉你在连接的流式套接字上谁在另一边
     - int getpeername(int sockfd,struct sockaddr* addr,int *addrlen);
     - sockfd是连接的流式套接字
     - addr保存着另一边的信息
     - 失败返回-1
  - gethostname()函数：
     - 需要头文件unistd.h
     - 获取主机名
     - int gethostname(char *hostname,size_t size);
     - hostname返回主机名，size是hostname数组的字节长度
  - 域名服务(DNS):
     - 从域名得到IP地址：调用gethostbyname()函数
     - 需要头文件netdb.h
     - struct hostent *gethostbyname(const char *name);
     ```cpp
     struct hostent{
      char *h_name;
      char **h_aliases;
      int h_addrtype;
      int h_length;
      char **h_addr_list;
      };
      ```
      - 其中h_name表示地址的正式名称
      - h_aliases表示地址预备名称的指针
      - h_addrtype表示地址类型，一般是AF——INFT
      - h_length表示地址的长度(16btyes)
      - h_addr_list表示主机网络地址指针，为网络字节顺序
      - 返回一个指向hostent的指针，失败返回NULL,但不会设置errno
  - 客户端-服务器：
      - 在网络上的所有东西都是在处理客户进程和服务器进程的交谈，举个telnet例子，当你用telnet通过23号端口登录到主机，主机上运行一个程序telnetd激活，他处理这个连接，显示登录界面等等。
      - 客户端-服务器端之间可以使用流式套接字也可以使用数据报套接字，一些很好的例子有：telnet-telnetd，ftp-ftpd和bootp-bootpd。每次你使用ftp时，在远端都有一个ftpd为你服务。
      - 一般，服务器等待连接，accept一个连接后用fork()创建一个子进程去处理它。
  - 简单的服务器实例：
      - 服务端：参见server1.cpp
      - 客户端：参见client1.cpp
      - 服务端使用fork()：参见server2.cpp
  - 阻塞：
      - accept()阻塞，recv()阻塞、他们之所以允许这么做，是因为当你第一次调用socket()建立套接字描述符的时候，内核就将它设置为阻塞。
      - 如果你不想套接字阻塞，你就要调用函数fcntl():
      ```cpp
      sockfd = socket(AF_INET,SOCK_STREAM,0);
      fcntl(sockfd,F_SETFL,O_NONBLOCK);
      ```
      - 这样将套接字设为非阻塞，你能够忙轮询套接字以获得信息，如果你尝试从一个非阻塞套接字读信息并且没有任何数据，将返回-1并将error设为EWOULDBLOCK
      - 非阻塞忙轮询并不是个好主意，因为将浪费大量CPU时间，为此的解决办法可以是设置一个中间层，也就是一个代理select去完成轮询工作，
  - 多路同步I/O---select():
      - 需要头文件sys/time.h，sys/time.h，unistd.h
      - 可以监听多个描述符，可以得知哪个套接字准备读，哪个套接字准备写，哪个套接字发生exception
      - int select(int numfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *timeout);
      - 这个函数监视一系列描述符：readfds，writefds和exceptfds
      - FD_ZERO（fd_set *set）清除一个文件描述符集合
      - FD_SET(int fd,fd_set *set)添加fd到集合
      - FD_CLR(int fd,fd_set *set)从集合中移去fd
      - FD_ISSET(int fd,fd_set *set)测试fd是否在集合中
      - struct timeval允许设定一个时间，如果时间到了，select()还没有找到一个准备好的文件描述符，它将返回让你处理
      ```cpp
      struct timeval{
        int tv_sec;//秒
        int tv_usec;//微妙
      };
      ```
      - 如果将时间设为0，select()将立即超时，这样就可以有效地轮询一次集合中的文件描述符
      - 如果将时间设为NULL，将永远不会发生超时，即一直等到第一个就绪的文件描述符
      - select()实例：
      ```cpp
      #include <sys/time.h>
      #include <sys/types.h>
      #include <unistd.h>
      #include <iostream>
      #define STDIN 0 /* file descriptor for standard input */
      using namespace std;
      int main(){
        struct timeval tv;
        fd_set readfds;
        tv.tv_sec = 2;
        tv.tv_usec = 500000;
        FD_ZERO(&readfds);
        FD_SET(STDIN, &readfds);
     /* don't care about writefds and exceptfds: */
        select(STDIN+1, &readfds, NULL, NULL, &tv);
        if (FD_ISSET(STDIN, &readfds))
          cout<<"A key was pressed!\n"<<endl;
        else
          cout<<"Timed out.\n"<<endl;;
      }
      ```
      - 2.5秒超时，除非在此之前按下回车
  - ttcp：
      - 涉及到write： ssize_t write(int fd,const void *buf,size_t nbytes);
      - 从buf中数据写入fd，返回值一般等于nbytes，否则就是出错，常见的出错原因是磁盘空间满了或者超过文件大小限制，正因为可能出错所以写一个write_n来确保写入指定长度的数据
      - 通过实例测得吞吐量为42.8755Mb/s
      - 测试结果：
      
       message length       |quantity     |bandwidth(Mb/s)   |
       ---------------------|-------------|------------------|
       1024                 | 1024        |10.5              |
       2048                 | 1024        |13.4              |
       4096                 | 1024        |21.4              |
       8192                 | 1024        |42.8              |
       16384                | 1024        |51.6              |
       32768                | 1024        |82.5              |
       65536                | 1024        |83.9              |
       131072               | 1024        |106.8             |
       262144               | 1024        |106.8             |
       
       - 结论：应用层消息一般不会很大，消息数量不影响传输速度，而ttxp所测带宽和消息的大小有关，消息越小，传输延迟的影响越大。
  - setsockopt()函数:
      - 在TCP连接中，recv等函数默认为阻塞模式，即直到有数据来之前函数不会返回，而我们有时则需要一种超时机制使其在一定时间后返回而不管是否有数据到来，这里我们就会用到setsockopt()函数。
      - int setsockopt(int sockfd,int level,int optname,void* optval,socklen_t* optlen);
      - 例如：setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&tv_out,sizeof(tv_out));这样就设定了超时机制，tv_out类型为timeval，当超过tv_out设定的时间而没有数据到来时recv()就会返回0值。
      - 再如：setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
      - 其中SOL_SOCKET为基本套接口，SO_REUSEADDR为允许重用本地地址和端口
      - 一般来说，一个端口释放后会等待两分钟之后才能再被使用，而SO_REUSEADDR可以让端口释放后立即就可以被再次使用
      - SO_REUSEADDR用于对TCP套接字处于TIME_WAIT状态下的socket，才可以被重复绑定使用
      - server程序总是应该在调用bind()之前设置SO__REUSEADDR套接字选项     
  - reinterpret_cast<type>
      - type必须是一个指针、引用、算数类型、函数指针或者成员指针。他可以把一个指针转换成一个整数，也可以把一个整数转换成一个指针。
      - 必须谨慎使用
  - write()函数
      - 将内存中buf中的数据发送给fd指向的文件，接下来什么时候将TCP发送缓冲区的数据发出给主机，什么时候被对方主机读取，这些是我们无法控制的。
      - size_t write(int sockfd,char *buf,size_t n);
      - 由于TCP是一个提供可靠数据传输的协议，发送端需要对已收到的报文确认信息，也就是说，发送端将内核send buffer中的数据发送到网络后并不会立即清除send buffer，必须等待收到对方主机的确认信息后才会清除。
      - 接收端从sockfd中接收数据后放入自己的receive buffer中，并对receive buffer中的数据返回一个确认信息。发送端收到对数据的ACK后才清除自己的send buffer。如果接收端接收端没有将receive buffer中的数据及时确认清除而导致receive buffer中的数据填满，由于滑动窗口协议的作用，接收端不会再从sockfd上读取数据，进而给发送端返回的报文段中阻止发送端发送数据。发送端仍然发送数据，直到发送端的send buffer被填满，write函数被阻塞。
      - 在每个TCP报文中，都会有一个字段叫cwd：来告知对方自己receive buffer的大小，对方收到报文后会根据cwd来判断是否还要发送数据。
      - 总的来说就是接收端接受数据的速度赶不上发送端发送数据的速度，接收端通过报文中的cwd告诉发送端不要再发送数据了，send buffer被填满而引发阻塞。
      - 实例分析：echo：客户端发送20M数据给服务端，服务端receive buffer不足20M，比如只读了10M，然后发送回给客户端，客户端收到cwd，不会再发送数据，然后就会填满send buffer，且write没结束不会read，所以客户端也会阻塞。
 - echo
      - 见echo_server.cpp 和 echo_client.cpp
 - read()函数
      - 从fd文件中读取n个字节的数据到内存的buf中，功能就是将内核中TCP接受缓冲区中的数据拷贝到内存数组buf中。
      - size_t read(int fd,char* buf,size_t n);
      - 阻塞原理：read函数发生阻塞是因为TCP的receive buffer中没有数据。也就是说发送端的数据还没有发送过来。
 - write和read行为的区别：
      - 当TCP的接收缓冲区中有数据的话，read就会对数据进行拷贝，而不是等到receive buffer中被填满时才会拷贝。
      - 而write却不同，只有当TCP的send buffer中能够存放内存buf的内容时才会将buf拷贝到send buffer中。
  - 整个通信过程中，任意一方都会有三个缓冲区：内存中存放数据的一个缓冲区，用于TCP通信的发送缓冲区和接受缓冲区，通过报文中cwd来控制传输的数据量
