#include <sts/types.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "tc_ex.h"
#include "tc_monitor.h"

using namespace std;

namespace tars{
	//线程控制异常类
	struct TC_ThreadThreadControl_Exception : public TC_Exception{
		TC_ThreadThreadControl_Exception(const string &buffer):TC_Exception(buffer){}
		TC_ThreadThreadControl_Exception(const string &buffer,int err):TC_Exception(buffer,err){}
		~TC_ThreadThreadControl_Exception()throw(){}
	};
	//线程控制类
	class TC_ThreadControl{
	public:
		//构造，表示当前运行的线程，join和detach不能在该对象上调用
		TC_ThreadControl():_thread(pthread_self()){}
		//构造，explicit
		TC_ThreadControl(pthread_t thread):_thread(thread){}
		//使一个线程等待另一个线程结束,不能在当前线程id上调用，阻塞
		void join(){
			if(pthread_self() == _thread)//要启动的线程不能为自身线程
				throw TC_ThreadThreadControl_Exception("cant be called in the same thread");
			int rc = pthread_join(_thread,&ignore);
			if(rc != 0)
				throw TC_ThreadThreadControl_Exception("pthread_join error",rc);
		}
		//分离线程，不在当前线程id上调用,非阻塞
		void detach(){
			if(pthread_self() == _thread)//要启动的线程不能为自身线程
				throw TC_ThreadThreadControl_Exception("cant be called in the same thread");
			int rc = pthread_detach(_thread);
			if(rc != 0)
				throw TC_ThreadThreadControl_Exception("pthread_detach error",rc);
		}
		//获取需启动的线程id
		pthread_t id() const {
			return _thread;
		}
		//休息多少毫秒
		static void sleep(long millsecond){
			struct timespec ts;
			ts.tv_sec = millsecond / 1000;
			ts.tc_nsec = (millsecond % 1000) * 1000000;
			nanosleep(&ts,0);
		}
		//交出当前线程的控制权
		static void yield(){
			sched_yield();
		}
	private: 
		pthread_t _thread;	
	}；

	class TC_Runnable{
	public:
		virtual ~TC_Runnable(){}
		virtual void run() = 0;//纯虚函数，不能创建实例，其子类也必须实现才能创建实例
	}；

	//线程基类，所有自定义线程继承于该类，同时实现run接口即可
	class TC_Thread : public TC_Runnable{
	public:
		//构造函数
		TC_Thread():_running(false),_tid(-1){}
		//析构函数
		virtual ~TC_Thread(){}
		//创建线程并等待，并返回一个控制该线程的控制类，可以用那个控制类的join或detach来起线程
		TC_ThreadControl start(){
			TC_ThreadLock::Lock sync(_lock);
			if(_running)
				throw TC_ThreadThreadControl_Exception("thread has start");
			int ret = pthread_create(&_tid,0,(void*(*)(void*))&threadEntry,(void*)this);
			if(ret != 0)
				throw TC_ThreadThreadControl_Exception("thread start error",ret);
			_lock.wait();
			return TC_ThreadControl(_tid);
		}
		//获取线程控制类
		TC_ThreadControl getThreadControl() const {
			return TC_ThreadControl(_tid);
		}
		//判断线程是否存活
		bool isAlive() const {
			return _running;
		}
		pthread_t id(){return _tid;}
	protected:
		//线程入口
		static void threadEntry(TC_Thread *pThread){
			pThread->_running = true;
			{
				TC_ThreadLock::Lock sync(pThread->_lock);
				pThread->_lock.notifyAll();//通知该锁上的所有线程起来
			}
			try{
				pThread->run();
			}
			catch(...){
				pThread->_running = false;
				throw;
			}
			pThread->_running = false;
		}
		//运行
		virtual void run() = 0;
	protected:
		//是否在运行
		bool _running;
		//线程ID
		pthread_t _tid;
		//线程锁
		TC_ThreadLock _lock;
	}
}