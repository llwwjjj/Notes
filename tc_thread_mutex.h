#include "_mutex.h"
#include <iostream>
#include <string.h>
using namespace std;
namespace tars{
	class TC_ThreadCond;

	//互斥鎖異常類
	struct TC_ThreadMutex_Exception : public TC_Lock_Exception{
		TC_ThreadMutex_Exception(const string &buffer):TC_Lock_Exception(buffer){}
		TC_ThreadMutex_Exception(const string &buffer,int err):TC_Lock_Exception(buffer,err){}
		~TC_ThreadMutex_Exception()throw(){}
	};

	//互斥锁，同一个线程不可以重复加锁，通常不直接使用，和TC_Monitor配合使用，即TC_ThreadLock,
	//PTHREAD_MUTEX_ERRORCHECK 检错锁，如果同一个线程请求同一个锁，则返回EDEADLK
	//否则与PTHREAD_MUTEX_TIMED类型动作相同，这样保证了当不允许多次加锁时不会出现最简单的死锁
	class TC_ThreadMutex{
	public:
		//构造函数创建并设置锁属性
		TC_ThreadMutex(){
			int rc = 0;
			//创建锁属性
			pthread_mutexattr_t attr;
			//初始化锁属性
			rc = pthread_mutexattr_init(&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_init error");
			//设置锁属性
			rc = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_ERRORCHECK);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_settype error");
			//动态创建锁对象
			rc = pthread_mutex_init(&_mutex,&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutex_init error");
			//销毁锁属性
			rc = pthread_mutexattr_destroy(&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_destroy error");	
		}
		//析构函数销毁锁对象
		~TC_ThreadMutex(){
			int rc = 0;
			rc = pthread_mutex_destroy(&_mutex);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_destroy error");
		}
		//加锁
		void lock() const {
			int rc = pthread_mutex_lock(&_mutex);
			if(rc != 0)
				if(rc == EDEADLK)
					throw TC_ThreadMutex_Exception("pthread_mutex_lock dead lock error");
				else
					throw TC_ThreadMutex_Exception("pthread_mutex_lock error");
		}
		//尝试加锁
		bool tryLock() const {
			int rc = pthread_mutex_trylock(&_mutex);
			if(rc != 0 && rc != EBUSY)
				if(rc == EDEADLK)
					throw TC_ThreadMutex_Exception("pthread_mutex_tryLock dead lock error");
				else
					throw TC_ThreadMutex_Exception("pthread_mutex_tryLock error");
			return (rc == 0);
		}
		//解锁
		bool unlock() const {
			int rc = pthread_mutex_unlock(&_mutex);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutex_unlock error");
		}
		//加锁后调用unlock是否会解锁，给monitor用的，永远返回true
		bool willUnlock() const {return true;}
	protected:
		//禁止拷贝
		TC_ThreadMutex(const TC_ThreadMutex&);
		TC_ThreadMutex& operator=(const TC_ThreadMutex&);
		int count() const{ return 0;}
		void count(int c){}
		//友元
		friend class TC_ThreadCond;
	protected:
		mutable pthread_mutex_t _mutex;
	};

	//线程锁类,PTHREAD_MUTEX_RECURSIVE 嵌套锁，允许同一个线程对同一个锁成功获得多次
	//并通过多次unlock解锁。如果是不同线程请求，则在加锁线程解锁时重新竞争
	//在通过逻辑使他只能lock一次
	class TC_ThreadRecMutex{
	public:
		//构造函数
		TC_ThreadRecMutex():_count(0){
			int rc = 0;
			//创建锁属性
			pthread_mutexattr_t attr;
			//初始化锁属性
			rc = pthread_mutexattr_init(&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_init error");
			//设置锁属性
			rc = pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_settype error");
			//动态创建锁对象
			rc = pthread_mutex_init(&_mutex,&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutex_init error");
			//销毁锁属性
			rc = pthread_mutexattr_destroy(&attr);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_destroy error");	
		}
		//析构函数，先根据锁的数量解锁
		~TC_ThreadRecMutex(){
			while(_count)
				unlock();
			int rc = pthread_mutex_destroy(&_mutex);
			if(rc != 0)
				throw TC_ThreadMutex_Exception("pthread_mutexattr_destroy error");
		}
		//加锁，由程序控制它只能创建一个锁，多创建则解掉
		int lock() const {
			int rc = pthread_mutex_lock(&_mutex);
			if(rc != 0)
					throw TC_ThreadMutex_Exception("pthread_mutex_lock error");
			if(++_count > 1){
				rc = pthread_mutex_unlock(&_mutex);
				if(rc != 0)
					throw TC_ThreadMutex_Exception("pthread_mutex_unlock error");
			}
			return rc;
		}
		//解锁，没锁不报错
		int unlock() const {
			if(--_count == 0){
				int rc = pthread_mutex_unlock(&_mutex);
				return rc;
			}
			return 0;
		}
		//尝试加锁
		bool tryLock() const {
			int rc = pthread_mutex_trylock(&_mutex);
			if(rc != 0)
				if(rc != EBUSY)
					pthread_mutex_trylock(&_mutex);
			else if(++_count > 1){
				rc = pthread_mutex_unlock(&_mutex);
				if(rc != 0)
					throw TC_ThreadMutex_Exception("pthread_mutex_unlock error");
			}
			return (rc == 0);
		}
		//加锁后调用unlock是否会解锁，给TC_Monitor用的
		bool willUnlock() const {return _count == 1;}
	protected:
		//禁止拷贝
		TC_ThreadRecMutex(const TC_ThreadRecMutex&);
		TC_ThreadRecMutex& operator=(const TC_ThreadRecMutex&);
		//友元
		friend class TC_ThreadCond;
		//计数
		int count() const {
			int c = _count;
			_count = 0;
			return c;
		}
		void count(int c) const {
			_count = c;
		}
	private:
		//锁对象
		mutable pthread_mutex_t _mutex;
		//锁数量
		mutable int _count;
	};
}