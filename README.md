# Notes
- 随笔，学习记录
-------------
目录
-------------
* __ attribute __
  - GNU C 中关键字__attribute__可以设置函数属性，变量属性和类型属性。使用方法__attribute__((__参数__));
也可以对struct或union进行属性设置，使用方法 struct xxx{int x;}__ attribute __((__参数__));
参数可以为：aligned，packed，transparent_union，unused，may_alias.\
    aligned:设定一个指定大小的对齐格式，如"
    ```cpp
    struct xxx {
      int x[3];
    };
    ```
    强制设定struct的变量在分配空间时采用8字节对齐方式，但如果超过连接器最大支持字节数是无济于事的。\
    packed:aligned属性使被设置的对象占用更多的空间，相反的，packed可以减小对象占用的空间。
  
* 智能指针类 tc_autoptr.h
* 异常类 tc_ex.h
