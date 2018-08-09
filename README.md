# Notes
- 随笔，学习记录
-------------
目录
-------------
* __ attribute __
  - GNU C 中关键字__attribute__可以设置函数属性，变量属性和类型属性。使用方法__attribute__((__参数__));
也可以对struct或union进行属性设置，使用方法 struct xxx{int x;}__ attribute __((__参数__));
参数可以为：aligned，packed，transparent_union，unused，may_alias.\
    \
    aligned: 设定一个指定大小的对齐格式，而且必须是2的指数倍，如"
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
    \
    packed: aligned属性使被设置的对象占用更多的空间，相反的，packed可以减小对象占用的空间。测试：
    ```cpp
    struct aaa {
      char a;
      short b;
      int c;
    }__attribute__((__packed__)) aa;
    cout<<sizeof(aa)<<endl;//得到7，因为1+2+4=7
    ```
    \
    __attribute__改变函数属性，如：
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
* sockaddr，sockaddr_in和in_addr
  - sockaddr: sockaddr是通用的socket地址，此数据结构用做bind，connect，recvfrom，sendto等函数的参数，指明地址信息。但一般编程中并不直接对此数据结构操作，而是使用另一个与sockaddr等价的数据结构sockaddr_in。
    ```cpp
    struct sockaddr{
      unsigned short sa_family;
      char sa_data[14];
    };
   ```
    其中sa_family是地址家族，一般都是AF_xxx的形式，代表tcp/ip协议族。
    sa_data是14字节协议地址。
    \
    sockaddr_in: 
* 智能指针类 tc_autoptr.h
* 异常类 tc_ex.h
