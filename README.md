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
    aligned: 设定一个指定大小的对齐格式，如"
    ```cpp
    struct xxx {
      int x[3];
    };__attribute__((__aligned(8)));
    ```
    强制设定struct的变量在分配空间时采用8字节对齐方式，但如果超过连接器最大支持字节数是无济于事的。测试：
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
    packed: aligned属性使被设置的对象占用更多的空间，相反的，packed可以减小对象占用的空间。
  
* 智能指针类 tc_autoptr.h
* 异常类 tc_ex.h
