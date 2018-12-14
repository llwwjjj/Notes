# Notes
- FieldGenerator相关随笔，学习记录
-------------
目录
-------------
* __VA_ARGS__
  - #在宏定义中可以将参数转换成字符串
  ```cpp
    #define P1(A) printf("%s,%d\n",#A,A);
    #define P2(A) printf(""#A"");
    int main(int argc, char const *argv[])
    {
	    int a = 1, b = 2;
	    P1(a);
	    P2(b);
	    P1(a+b);
	    return 0;
    }
  ```
  - ##在宏定义中可以起到粘合作用
  - __VA_ARGS__ 是一个可变参数的宏
  ```cpp
    #define myprint(...) printf(__VA_ARGS__)
    #define myprint2(x,...) printf(x,__VA_ARGS__)
    #define myprint3(x,...) printf(x,##__VA_ARGS__)
    int main(int argc, char const *argv[])
    {
	    int i =2 ,j = 3;
	    myprint("i=%d,j=%d\n",i,j);
	    myprint2("i=%d,j=%d\n",i,j);//必须有两个或以上参数
	    myprint3("i=%d\n",i);
	    return 0;
    }
  ```
