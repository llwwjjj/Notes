#include <string.h>
#include <iostream>
#include "tc_ex.h"
using namespace std;
//和timeprovider相关的没写
namespace tars{
	class TC_ThreadMutex;

	//条件锁异常类
	struct TC_ThreadCond_Exception : public TC_Exception{
		TC_ThreadCond_Exception(const string &buffer):TC_Exception(buffer){}
		TC_ThreadCond_Exception(const string &buffer,int err):TC_Exception(buffer,err){}
		~TC_ThreadCond_Exception()throw(){}
	};

	//条件锁，所有锁可以在上面等待信号发生
	//和TC_ThreadMutex，TC_ThreadRecMutex配合使用
	//通常不直接使用，而是使用TC_ThreadLock/TC_ThreadRecLock
	class TC_ThreadCond{
	public:
		//构造函数
		TC_ThreadCond(){
			int rc = 0;
			//创建锁属性
			pthread_condattr_t attr;
			//初始化锁属性
			rc = pthread_condattr_init(&attr);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_condattr_init error");
			//动态初始化条件变量
			rc = pthread_cond_init(&_cond,&attr);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_cond_init error");
			//销毁锁属性
			rc = pthread_condattr_destroy(&attr);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_condattr_destroy error");	
		}
		//析构函数
		~TC_ThreadCond(){
			int rc = pthread_cond_destroy(&_cond);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_cond_destroy error");	
		}
		//发送信号，等待在该条件上的一个线程醒来
		void signal(){
			int rc = pthread_cond_signal(&_cond);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_cond_signal error");	
		}
		//发广播，等待在该条件的线程都会醒来
		void broadcast(){
			int rc = pthread_cond_broadcast(&_cond);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_cond_broadcast error");
		}
		//无限制等待
		template<class Mutex>
		void wait(const Mutex& mutex) const {
			int c = mutex.count();
			int rc = pthread_cond_wait(&_cond,&mutex._mutex);//阻塞，等待事件发生
			mutex.count(c);
			if(rc != 0)
				throw TC_ThreadCond_Exception("pthread_cond_wait error");
		}
	private:
		//条件变量
		mutable pthread_cond_t _cond;
	};
}