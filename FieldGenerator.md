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
	    myprint3("i=%d\n",i);//可以只有一个参数
	    return 0;
    }
  ```
  - progress
  ```cpp	
	//    DCache_Struct(Rank,uid,sex,age)
	//--->MAKE_META(Rank,GET_ARG_COUNT(uid,sex,age),uid,sex,age)
	//--->GET_ARG_COUNT(uid,sex,age)->GET_ARG_COUNT_INNER(uid,sex,age,119,118,117,...0)
	//  ->MARCO_EXPAND(3)
	122 121 120 119 ... 5 4   3  2  1   0
	1    2   3   4           120   122
				    N
	//这个操作使得DCache_Struct里的字段数最多为119个
	//--->MAKE_META_DATA(Rank,3,uid,sex,age)
	//--->namespace Rank {MARCO_EXPAND(MARCO_CONCAT(CON_STR,3)(uid,sex,age))}
	//--->namespace Rank {MARCO_EXPAND(CON_STR_3)(uid,sex,age)}
	//--->namespace Rank {CON_STR_3(uid,sex,age)}
	//namespcae Rank {static const DCache::Field uid("uid");static const DCache::Field sex("sex");static const DCache::Field age("age");}

   ```
