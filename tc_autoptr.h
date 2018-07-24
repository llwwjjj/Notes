//
//  tc_autoptr.h
//  Util
//
//  Created by 练文健 on 2018/7/23.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef tc_autoptr_h
#define tc_autoptr_h

//智能指针类，智能指针智不能相互引用，否则内存泄漏。
#include "tc_atomic.h"
#include "tc_ex.h"
namespace tars{
    struct TC_AutoPtrNull_Exception:public TC_Exception{
        TC_AutoPtrNull_Exception(const string &buffer):TC_Exception(buffer){}
        ~TC_AutoPtrNull_Exception()throw(){}
    };
    template<class T>
    class TC_HandleBaseT{//智能指针基类，所有需要智能指针支持的类都需要从该对象继承
    public:
        typedef T automic_type;
    protected:
        automic_type _atomic;//计数
        bool _bNoDelete;//是否自动删除
    protected://不想让用户直接构造这个类，希望用户只能构造它的子类
        TC_HandleBaseT():_atomic(0),_bNoDelete(false){}/构造
        TC_HandleBaseT(const TC_HandleBaseT&):_atomic(0),_bNoDelete(false){}//拷贝构造
        virtual ~TC_HandleBaseT(){}//析构
    public:
        TC_HandleBaseT& operator=(const TC_HandleBaseT&){return *this;}//拷贝赋值
        
        void incRef(){_atomic.inc_fast();}//增加计数
        void decRef(){//减少计数，=0时删除数据，释放对象
            if(_atomic.dec_and_test()&&！_bNoDelete){
                _bNoDelete=true;
                delete *this;
            }
        }
        int getRef()const {return _atomic.get();}//获取计数值
        void setNoDelete(bool b){_bNoDelete=b;}//设置是否删除
    };
    template<>
    inline void TC_HandleBaseT<int>::incRef(){++_atomic;} //模板特化,T一般是TC_Atomic类型或者int
    template<>
    inline void TC_HandleBaseT<int>::decRef(){
        if(--_atomic==0 && !_bNoDelete){
            _bNoDelete=true;
            delete *this;
        }
    }
    template<>
    inline int getRef()const {return _atomic;}
    
    typedef TC_HandleBaseT<TC_Atomic> TC_HandleBase;
    
    //智能指针模板类，可以放在容器中，且线程安全的智能指针
    template<class T>
    class TC_AutoPtr{
    public:
        typedef T element_type;
        T* _ptr;
        
        TC_AutoPtr(T* p=0){  //构造函数，用原生指针初始化，计数+1
            _ptr=p;
            if(_ptr)
                _ptr->incRef();
        }
        template<class Y>
        TC_AutoPtr(const TC_AutoPtr<Y>& r){//构造函数，用其他智能指针的原生指针初始化，计数+1
            _ptr=r._ptr;
            if(_ptr)
                _ptr->incRef();
        }
        
        TC_AutoPtr(const TC_AutoPtr& r){//拷贝构造函数，计数+1
            _ptr=r._ptr;
            if(_ptr)
                _ptr->incRef();
        }
        
        ~TC_AutoPtr(){//析构函数
            if(_ptr)
                _ptr->decRef();
        }
        
        TC_AutoPtr& operator=(T* p){
            if(_ptr!=p){
                if(p)
                    p->incRef();
                T* ptr=_ptr;
                _ptr=p;
                if(ptr)
                    ptr->decRef();
                return *this;
            }
            
        }
        template<class Y>
        TC_AutoPtr& operator=(const TC_AutoPtr<Y>& r){
            if(_ptr!=r._ptr){
                if(r._ptr)
                    r._ptr->incRef();
                T* ptr=_ptr;
                _ptr=r._ptr;
                if(ptr)
                    ptr->decRef();
            }
            return *this;
        }
        TC_AutoPtr& operator=(const TC_AutoPtr& r){
            if(_ptr!=r._ptr){
                if(r._ptr)
                    r._ptr->incRef();
                T* ptr=_ptr;
                _ptr=r._ptr;
                if(ptr)
                    ptr->decRef();
            }
            return *this;
        }
        
        //将其他类型的智能指针转换成当前类型的智能指针
        template<class Y>
        static TC_AutoPtr dynamicCast(const TC_AutoPtr<Y>& r){return TC_AutoPtr(dynamicCast<T*>(r._ptr));}
        //将其他原生类型的指针转换成当前类型的智能指针
        template<class Y>
        static TC_AutoPtr dynamicCast(Y* p){return TC_AutoPtr(dynamicCast<T*>(p));}
        
        T* get()const {return _ptr;}
        
        T* operator->()const{
            if(!_ptr)
                return throwNullHandleException();
            return _ptr;
        }
        T& operator*() const{
            if(!_ptr)
                return throwNullHandleException();
            return *_ptr;
        }
        operator bool() const{return _ptr?true:false;}//是否有效
        void swap(TC_AutoPtr& other){std::swap(_ptr,other._ptr);}
    protected:
        void throwNullHandleException() const{ throw TC_AutoPtrNull_Exception("autoptr null handle error"); }
    };
    
    template<class T,class U>
    inline bool operator==(const TC_AutoPtr<T>& lhs,TC_AutoPtr<U>& rhs){
        T* l=lhs.get();
        U* r=rhs.get();
        if(l && r)
            return *l==*r;
        else
            return !l && !r;
    }
    
    template<class T,class U>
    inline bool operator!=(const TC_AutoPtr<T>& lhs,TC_AutoPtr<U>& rhs){
        T* l=lhs.get();
        U* r=rhs.get();
        if(l && r)
            return *l!=*r;
        else
            return l || r;
    }
    
    template<class T,class U>
    inline bool operator<(const TC_AutoPtr<T>& lhs,TC_AutoPtr<U>& rhs){
        T* l=lhs.get();
        U* r=rhs.get();
        if(l && r)
            return *l < *r;
        else
            return !l && r;
    }
}

#endif /* tc_autoptr_h */
