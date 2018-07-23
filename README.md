# Tars-Notes
学习TARS，剖析源码。
目录
-------------
* [DAY1]
  * 原子计数类 tc_atomic.h
    ```cpp
    #ifndef tc_atomic_h
    #define tc_atomic_h
    namespace tars{
    __BEGIN_DECLS //c和c++混用，为了使两者保持互相兼容的过程调用，需要在extern C作为声明的一部分，为了简化，从而定义了这两个宏。
    #define TARS_LOCK "lock ;"
     typedef struct {volatile int counter;} tars_atomic_t;
    #define tars_atomic_read(v) ((v)->counter)
    #define tars_atomic_set(v,i) (((v)->counter) = (i))
    __END_DECLS
    
     class TC_Atomic{
     protected:
         tars_atomic_t _value;
     public:
         
         int set(int n){
             _value.counter=n;
             return n;
         }
        int get() const {return _value.counter;}//获取值
        int add(int n){
            int i=n;
            __asm__ __volatile__(
                                 TARS_LOCK "xaddl %0,%1;":"=r"(i):"m"(_value.counter),"0"(i));
            return i+n;
        }//需要原子性操作
        int sub(int n){return add(-1);}
        int inc(){return add(1);}
        int dec(){return sub(1);}
        
        
        
        operator int() const{return get();}
        TC_Atomic(int at=0){set(at);}//构造函数，初始化为0
        TC_Atomic& operator++(){
            inc();
            return *this;
        }
        TC_Atomic& operator--(){
            dec();
            return *this;
        }
        TC_Atomic& operator+=(int n){
            add(n);
            return *this;
        }
        TC_Atomic& operator-=(int n){
            sub(n);
            return *this;
        }
        TC_Atomic& operator=(int n){
            set(n);
            return *this;
        }
      };
    }
    #endif /* tc_atomic_h */
    ```

  
