//
//  tc_ex.h
//  Util
//
//  Created by 练文健 on 2018/7/24.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef tc_ex_h
#define tc_ex_h

#include<stdexcept>
#include<string.h>
#include<stdlib.h>//free
#include<errno>//定义了几个宏，其和错误状态的报告有关
#include<execinfo.h>//包含三个函数用于获取当前线程的函数调用堆栈
//1, int backtrace(void **buffer,int size) 该函数用于获取当前线程调用堆栈，获取的信息会被存放在buffer中，他是一个指针列表。size用于指定buffer可以保存多少void*元素,返回小于等于size的实际获取的指针个数，在buffer中的指针实际是从堆栈中获取的返回地址。
//2，char** backtrace_symbols(void *const *buffer,int size)返回一个指向字符串数组的指针，大小与buffer相同，每个字符串包含了一个相对于buffer中对应元素的可打印信息。包括函数名，函数的偏移地址，和实际的返回地址。
//3, void backtrace_symbols_fd(void *const *buffer,int size,int fd)将上述信息写进描述符为fd的文件，每个函数一行，不需要调用malloc，因此适用于有可能调用失败的情况。
namespace tars{
    class TC_Exception:public exception{
    private:
        string _buffer;//异常相关信息
        int _code;//错误码
        
        void getBacktrace(){
            void* array[64];//buffer
            int nSize = backtrace(array,64);
            char **symbols=backtrace_symbols(array,nSize);
            for(int i=0;i<nSize;++i){
                _buffer+=symbols[i];
                _buffer+='\n';
            }
            free(symbols);
        }
        
    public:
        explicit TC_Exception(const string &buffer):_buffer(buffer),_code(0){/*getBacktrace();*/}
        TC_Exception(const string &buffer,int err){ //传入异常信息和错误码
            _buffer=buffer+":"strerror(err);//strerror将errno值映射为一个出错字符串，并返回此字符串的指针。
            _code=err;
            //getBacktrace();
        }
        virtual ~TC_Exception()throw(){}
        
        virtual const char* what() const throw(){return _buffer.c_str();}//c_str以char形式返回string
        
        int getErrCode(){return _code;} //获取错误码
    };
}

#endif /* tc_ex_h */
