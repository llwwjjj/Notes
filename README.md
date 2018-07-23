# Tars-Notes
学习TARS，剖析源码。
目录
-------------
* [DAY1]
  * 原子计数类 tc_atomic.h
    ```cpp
    #ifndef tc_atomic_h
    #define tc_atomic_h
    #include <stdint.h>
    namespace tars{
    __BEGIN_DECLS //c和c++混用，为了使两者保持互相兼容的过程调用，需要在extern C作为声明的一部分，为了简化，从而定义了这两个宏。
    #define TARS_LOCK "lock ;"
      typedef struct {volatile int counter;} tars_atomic_t;
    #define tars_atomic_read(v) ((v)->counter)
    #define tars_atomic_set(v,i) (((v)->counter) = (i))
    __END_DECLS
    
    }
    ```

  
